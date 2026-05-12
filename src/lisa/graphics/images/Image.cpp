//
// Created by kinami on 4/1/26.
//

#include "Image.h"

#include "ImageFormat.h"
#include "graphics/buffer/Buffer.h"
#include "graphics/context.h"
#include "utils/logging.h"

namespace lisa::graphics {
  Image::Image(
    const str& id,
    ImageFormat format,
    vk::ImageUsageFlags usage,
    const vec3& size,
    vk::ImageType type,
    uint32 mips,
    vk::ImageLayout initial_layout,
    const vma::AllocationCreateInfo& allocation_ci
  ) :
    NamedVkObject(id),
    size_(size),
    mips_(mips),
    format_(format),
    usage_(usage),
    type_(type),
    initial_layout_(initial_layout) {
    allocate(allocation_ci);
  }

  void Image::allocate(const vma::AllocationCreateInfo& allocation_ci) {
    const vk::ImageCreateInfo image_ci{
      .imageType = type_,
      .format = format_,
      .extent =
        vk::Extent3D{
          static_cast<uint32_t>(size_.x),
          static_cast<uint32_t>(size_.y),
          static_cast<uint32_t>(size_.z)
        },
      .mipLevels = mips_,
      .arrayLayers = 1,
      .samples = vk::SampleCountFlagBits::e1,
      .tiling = vk::ImageTiling::eOptimal,
      .usage = usage_,
      .sharingMode = vk::SharingMode::eExclusive,
      .initialLayout = initial_layout_
    };
    set(context::allocator().create_image(image_ci, allocation_ci));
  }

  const vk::raii::ImageView& Image::view(const ImageViewDesc& desc) const {
    if (const auto it = views_.find(desc); it != views_.end())
      return it->second;

    const vk::ImageViewCreateInfo view_ci{
      .image = static_cast<const vk::Image&>(*this),
      .viewType = desc.type,
      .format = desc.format,
      .subresourceRange = desc.range
    };

    auto newView = context::device().create_image_view(view_ci);
    auto [it, inserted] = views_.emplace(desc, std::move(newView));

    if constexpr (build::debug) {
      const auto& view = it->second;

      const auto handle = static_cast<vk::raii::ImageView::CType>(*view);
      uint64_t vk_handle;
      std::memcpy(&vk_handle, &handle, sizeof(handle));

      view_debug_names_[desc] = logging::genid(id(), "view");

      const vk::DebugUtilsObjectNameInfoEXT name_info{
        .objectType = view.objectType,
        .objectHandle = vk_handle,
        .pObjectName = view_debug_names_[desc].c_str()
      };

      context::device()->setDebugUtilsObjectNameEXT(name_info);
    }

    return it->second;
  }

  Image Image::from_data(
    const str& id,
    const void* data,
    size_t size,
    const vec3& extent,
    ImageFormat format,
    vk::ImageUsageFlags usage,
    const CommandBuffer& cmdb,
    uint32 mip_levels
  ) {
    auto transfer_buffer = Buffer(
      logging::genid(id, "transfer"),
      size,
      vk::BufferUsageFlagBits::eTransferSrc,
      {.flags = vma::AllocationCreateFlagBits::eMapped |
                vma::AllocationCreateFlagBits::eHostAccessSequentialWrite,
       .usage = vma::MemoryUsage::eAuto}
    );
    std::memcpy(transfer_buffer.mapped_data(), data, size);

    auto image = Image(
      id,
      format,
      usage |
        vk::ImageUsageFlagBits::eTransferDst |
        vk::ImageUsageFlagBits::eTransferSrc,
      extent,
      vk::ImageType::e2D,
      mip_levels,
      vk::ImageLayout::eUndefined,
      DEFAULT_ALLOCATION_CI
    );

    const auto aspect_mask = format.aspect_mask();

    vk::ImageMemoryBarrier2 barrier{
      .srcStageMask = vk::PipelineStageFlagBits2::eNone,
      .srcAccessMask = vk::AccessFlagBits2::eNone,
      .dstStageMask = vk::PipelineStageFlagBits2::eTransfer,
      .dstAccessMask = vk::AccessFlagBits2::eTransferWrite,
      .oldLayout = vk::ImageLayout::eUndefined,
      .newLayout = vk::ImageLayout::eTransferDstOptimal,
      .image = image,
      .subresourceRange = {
        .aspectMask = aspect_mask,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1
      }
    };
    vk::DependencyInfo dependency_i{
      .imageMemoryBarrierCount = 1, .pImageMemoryBarriers = &barrier
    };
    cmdb->pipelineBarrier2(dependency_i);

    const vk::BufferImageCopy copy_region{
      .bufferOffset = 0,
      .bufferRowLength = 0,
      .bufferImageHeight = 0,
      .imageSubresource =
        {.aspectMask = aspect_mask,
         .mipLevel = 0,
         .baseArrayLayer = 0,
         .layerCount = 1},
      .imageExtent = {
        .width = static_cast<uint32>(extent.x),
        .height = static_cast<uint32>(extent.y),
        .depth = static_cast<uint32>(extent.z)
      }
    };
    cmdb->copyBufferToImage(
      transfer_buffer,
      image,
      vk::ImageLayout::eTransferDstOptimal,
      {copy_region}
    );

    int32 mip_width = static_cast<int32>(extent.x);
    int32 mip_height = static_cast<int32>(extent.y);

    for (uint32 i = 1; i < mip_levels; i++) {
      barrier.subresourceRange.baseMipLevel = i - 1;
      barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
      barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
      barrier.srcStageMask = vk::PipelineStageFlagBits2::eTransfer;
      barrier.srcAccessMask = vk::AccessFlagBits2::eTransferWrite;
      barrier.dstStageMask = vk::PipelineStageFlagBits2::eTransfer;
      barrier.dstAccessMask = vk::AccessFlagBits2::eTransferRead;
      cmdb->pipelineBarrier2(dependency_i);

      barrier.subresourceRange.baseMipLevel = i;
      barrier.oldLayout = vk::ImageLayout::eUndefined;
      barrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
      barrier.srcStageMask = vk::PipelineStageFlagBits2::eNone;
      barrier.srcAccessMask = vk::AccessFlagBits2::eNone;
      barrier.dstStageMask = vk::PipelineStageFlagBits2::eTransfer;
      barrier.dstAccessMask = vk::AccessFlagBits2::eTransferWrite;
      cmdb->pipelineBarrier2(dependency_i);

      vk::ImageBlit blit{
        .srcSubresource =
          {.aspectMask = aspect_mask,
           .mipLevel = i - 1,
           .baseArrayLayer = 0,
           .layerCount = 1},
        .dstSubresource = {
          .aspectMask = aspect_mask,
          .mipLevel = i,
          .baseArrayLayer = 0,
          .layerCount = 1
        }
      };

      blit.srcOffsets[0] = vk::Offset3D{0, 0, 0};
      blit.srcOffsets[1] = vk::Offset3D{mip_width, mip_height, 1};

      blit.dstOffsets[0] = vk::Offset3D{0, 0, 0};
      blit.dstOffsets[1] = vk::Offset3D{
        mip_width > 1 ? mip_width / 2 : 1,
        mip_height > 1 ? mip_height / 2 : 1,
        1
      };
      cmdb->blitImage(
        image,
        vk::ImageLayout::eTransferSrcOptimal,
        image,
        vk::ImageLayout::eTransferDstOptimal,
        blit,
        vk::Filter::eLinear
      );

      barrier.subresourceRange.baseMipLevel = i - 1;
      barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
      barrier.newLayout = vk::ImageLayout::eReadOnlyOptimal;
      barrier.srcStageMask = vk::PipelineStageFlagBits2::eTransfer;
      barrier.srcAccessMask = vk::AccessFlagBits2::eTransferRead;
      barrier.dstStageMask = vk::PipelineStageFlagBits2::eFragmentShader;
      barrier.dstAccessMask = vk::AccessFlagBits2::eShaderRead;
      cmdb->pipelineBarrier2(dependency_i);

      if (mip_width > 1) mip_width /= 2;
      if (mip_height > 1) mip_height /= 2;
    }

    barrier.subresourceRange.baseMipLevel = mip_levels - 1;
    barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
    barrier.newLayout = vk::ImageLayout::eReadOnlyOptimal;
    barrier.srcStageMask = vk::PipelineStageFlagBits2::eTransfer;
    barrier.srcAccessMask = vk::AccessFlagBits2::eTransferWrite;
    barrier.dstStageMask = vk::PipelineStageFlagBits2::eFragmentShader;
    barrier.dstAccessMask = vk::AccessFlagBits2::eShaderRead;
    cmdb->pipelineBarrier2(dependency_i);

    cmdb.keep_alive(std::move(transfer_buffer));

    return image;
  }

  Image Image::from_data(
    const str& id,
    const void* data,
    const size_t size,
    const vec3& extent,
    const ImageFormat format,
    const vk::ImageUsageFlags usage,
    const uint32 mip_levels
  ) {
    const auto cmdb = context::device().cmd_buffer();
    cmdb.begin_onetime();

    auto image =
      from_data(id, data, size, extent, format, usage, cmdb, mip_levels);

    cmdb->end();
    context::device().submit_cmd_buffer_with_fence(cmdb);

    return image;
  }
}
