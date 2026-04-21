//
// Created by kinami on 4/3/26.
//

#include "Texture.h"

#include "graphics/buffer/Buffer.h"
#include "stb_image.h"
#include "tinyexr.h"

#include <cstring>
#include <ktxvulkan.h>

namespace lisa::resources {
  graphics::Image Texture::load_ktx(const path& filepath) {
    ktxTexture* texture;
    ktxTexture_CreateFromNamedFile(
      filepath.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &texture
    );

    auto cmdb = graphics::context::device().cmd_buffer();

    cmdb.begin_onetime();

    auto transfer_buffer = graphics::Buffer(
      texture->dataSize,
      vk::BufferUsageFlagBits::eTransferSrc,
      {.flags = vma::AllocationCreateFlagBits::eMapped |
                vma::AllocationCreateFlagBits::eHostAccessSequentialWrite,
       .usage = vma::MemoryUsage::eAuto}
    );
    std::memcpy(
      transfer_buffer.mapped_data(), texture->pData, texture->dataSize
    );

    const auto format = graphics::ImageFormat(ktxTexture_GetVkFormat(texture));

    auto image = graphics::Image(
      format,
      vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
      {texture->baseWidth, texture->baseHeight, 1},
      vk::ImageType::e2D,
      texture->numLevels,
      vk::ImageLayout::eUndefined,
      {.usage = vma::MemoryUsage::eAuto}
    );

    const vk::ImageMemoryBarrier2 barrier_info{
      .srcStageMask = vk::PipelineStageFlagBits2::eNone,
      .srcAccessMask = vk::AccessFlagBits2::eNone,
      .dstStageMask = vk::PipelineStageFlagBits2::eTransfer,
      .dstAccessMask = vk::AccessFlagBits2::eTransferWrite,
      .oldLayout = vk::ImageLayout::eUndefined,
      .newLayout = vk::ImageLayout::eTransferDstOptimal,
      .image = image,
      .subresourceRange = {
        .aspectMask = vk::ImageAspectFlagBits::eColor,
        .levelCount = texture->numLevels,
        .layerCount = 1
      }
    };
    vk::DependencyInfo dependency_i{
      .imageMemoryBarrierCount = 1, .pImageMemoryBarriers = &barrier_info
    };
    cmdb->pipelineBarrier2(dependency_i);

    vector<vk::BufferImageCopy> copy_regions;
    for (auto i = 0; i < texture->numLevels; i++) {
      ktx_size_t offset = 0;
      ktxTexture_GetImageOffset(texture, i, 0, 0, &offset);

      const vk::BufferImageCopy buffer_image_copy{
        .bufferOffset = offset,
        .imageSubresource =
          {.aspectMask = vk::ImageAspectFlagBits::eColor,
           .mipLevel = static_cast<uint32>(i),
           .layerCount = 1},
        .imageExtent = {
          .width = texture->baseWidth >> i,
          .height = texture->baseHeight >> i,
          .depth = 1
        }
      };
      copy_regions.push_back(buffer_image_copy);
    }
    cmdb->copyBufferToImage(
      transfer_buffer, image, vk::ImageLayout::eTransferDstOptimal, copy_regions
    );

    const vk::ImageMemoryBarrier2 barrier_read_info{
      .srcStageMask = vk::PipelineStageFlagBits2::eTransfer,
      .srcAccessMask = vk::AccessFlagBits2::eTransferWrite,
      .dstStageMask = vk::PipelineStageFlagBits2::eFragmentShader,
      .dstAccessMask = vk::AccessFlagBits2::eShaderRead,
      .oldLayout = vk::ImageLayout::eTransferDstOptimal,
      .newLayout = vk::ImageLayout::eReadOnlyOptimal,
      .image = image,
      .subresourceRange = {
        .aspectMask = vk::ImageAspectFlagBits::eColor,
        .levelCount = texture->numLevels,
        .layerCount = 1
      }
    };
    dependency_i.pImageMemoryBarriers = &barrier_read_info;
    cmdb->pipelineBarrier2(dependency_i);

    cmdb->end();
    graphics::context::device().submit_cmd_buffer_with_fence(cmdb);

    ktxTexture_Destroy(texture);

    return image;
  }

  void Texture::generate_mipmaps(
    const void* pixel_data,
    size_t buffer_size,
    uint32 width,
    uint32 height,
    uint32 mip_levels,
    const graphics::Image& image
  ) {
    auto cmdb = graphics::context::device().cmd_buffer();
    cmdb.begin_onetime();

    auto transfer_buffer = graphics::Buffer(
      buffer_size,
      vk::BufferUsageFlagBits::eTransferSrc,
      {.flags = vma::AllocationCreateFlagBits::eMapped |
                vma::AllocationCreateFlagBits::eHostAccessSequentialWrite,
       .usage = vma::MemoryUsage::eAuto}
    );
    std::memcpy(transfer_buffer.mapped_data(), pixel_data, buffer_size);

    vk::ImageMemoryBarrier2 barrier{
      .srcStageMask = vk::PipelineStageFlagBits2::eNone,
      .srcAccessMask = vk::AccessFlagBits2::eNone,
      .dstStageMask = vk::PipelineStageFlagBits2::eTransfer,
      .dstAccessMask = vk::AccessFlagBits2::eTransferWrite,
      .oldLayout = vk::ImageLayout::eUndefined,
      .newLayout = vk::ImageLayout::eTransferDstOptimal,
      .image = image,
      .subresourceRange = {
        .aspectMask = vk::ImageAspectFlagBits::eColor,
        .baseMipLevel = 0,
        .levelCount = 1,
        .layerCount = 1
      }
    };
    vk::DependencyInfo dependency{
      .imageMemoryBarrierCount = 1, .pImageMemoryBarriers = &barrier
    };
    cmdb->pipelineBarrier2(dependency);

    const vk::BufferImageCopy copy_region{
      .imageSubresource =
        {.aspectMask = vk::ImageAspectFlagBits::eColor,
         .mipLevel = 0,
         .layerCount = 1},
      .imageExtent = {width, height, 1}
    };
    cmdb->copyBufferToImage(
      transfer_buffer, image, vk::ImageLayout::eTransferDstOptimal, copy_region
    );

    int32 mip_width = width;
    int32 mip_height = height;
    for (uint32 i = 1; i < mip_levels; i++) {
      barrier.subresourceRange.baseMipLevel = i - 1;
      barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
      barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
      barrier.srcStageMask = vk::PipelineStageFlagBits2::eTransfer;
      barrier.srcAccessMask = vk::AccessFlagBits2::eTransferWrite;
      barrier.dstStageMask = vk::PipelineStageFlagBits2::eTransfer;
      barrier.dstAccessMask = vk::AccessFlagBits2::eTransferRead;
      cmdb->pipelineBarrier2(dependency);

      barrier.subresourceRange.baseMipLevel = i;
      barrier.oldLayout = vk::ImageLayout::eUndefined;
      barrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
      barrier.srcStageMask = vk::PipelineStageFlagBits2::eNone;
      barrier.srcAccessMask = vk::AccessFlagBits2::eNone;
      barrier.dstStageMask = vk::PipelineStageFlagBits2::eTransfer;
      barrier.dstAccessMask = vk::AccessFlagBits2::eTransferWrite;
      cmdb->pipelineBarrier2(dependency);

      vk::ImageBlit blit{
        .srcSubresource =
          {.aspectMask = vk::ImageAspectFlagBits::eColor,
           .mipLevel = i - 1,
           .baseArrayLayer = 0,
           .layerCount = 1},
        .dstSubresource = {
          .aspectMask = vk::ImageAspectFlagBits::eColor,
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
      cmdb->pipelineBarrier2(dependency);

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
    cmdb->pipelineBarrier2(dependency);

    cmdb->end();

    graphics::context::device().submit_cmd_buffer_with_fence(cmdb);
  }

  graphics::Image Texture::load_jpg(const path& filepath) {
    int w, h, channels;
    stbi_set_flip_vertically_on_load(true);
    stbi_uc* pixels =
      stbi_load(filepath.c_str(), &w, &h, &channels, STBI_rgb_alpha);
    if (!pixels)
      logging::abort("Failed to load STB image: {}", filepath.string());

    uint32 width = static_cast<uint32>(w);
    uint32 height = static_cast<uint32>(h);
    const uint32 mip_levels =
      static_cast<uint32>(
        std::log2(static_cast<float>(std::max(width, height)))
      ) +
      1;
    const size buffer_size = width * height * 4;

    auto image = graphics::Image(
      graphics::ImageFormat(vk::Format::eR8G8B8A8Unorm),
      vk::ImageUsageFlagBits::eTransferDst |
        vk::ImageUsageFlagBits::eTransferSrc |
        vk::ImageUsageFlagBits::eSampled,
      {width, height, 1},
      vk::ImageType::e2D,
      mip_levels,
      vk::ImageLayout::eUndefined,
      {.usage = vma::MemoryUsage::eAuto}
    );

    generate_mipmaps(pixels, buffer_size, width, height, mip_levels, image);

    stbi_image_free(pixels);

    return image;
  }

  graphics::Image Texture::load_exr(const path& filepath) {
    float* out_rgba = nullptr;
    int w, h;
    const char* err = nullptr;

    if (const auto ret = LoadEXR(&out_rgba, &w, &h, filepath.c_str(), &err);
        ret != TINYEXR_SUCCESS) {
      if (err) {
        logging::abort("Failed to load EXR: {} - {}", filepath.string(), err);
        FreeEXRErrorMessage(err);
      }
    }

    auto width = static_cast<uint32>(w);
    auto height = static_cast<uint32>(h);
    const auto mip_levels =
      static_cast<uint32>(
        std::log2(static_cast<float>(std::max(width, height)))
      ) +
      1;
    const auto buffer_size = width * height * 4 * sizeof(float);

    auto image = graphics::Image(
      graphics::ImageFormat(vk::Format::eR32G32B32A32Sfloat),
      vk::ImageUsageFlagBits::eTransferDst |
        vk::ImageUsageFlagBits::eTransferSrc |
        vk::ImageUsageFlagBits::eSampled,
      {width, height, 1},
      vk::ImageType::e2D,
      mip_levels,
      vk::ImageLayout::eUndefined,
      {.usage = vma::MemoryUsage::eAuto}
    );

    generate_mipmaps(out_rgba, buffer_size, width, height, mip_levels, image);

    free(out_rgba);

    return image;
  }

  bool Texture::load_function() {
    if (const auto ext = path_.extension().string();
        ext == ".ktx" || ext == ".ktx2") {
      image_ = load_ktx(path_);
    } else if (ext == ".jpg" || ext == ".jpeg" || ext == ".png") {
      image_ = load_jpg(path_);
    } else if (ext == ".exr") {
      image_ = load_exr(path_);
    }

    sampler_ = graphics::Sampler(static_cast<float>(image_.mipmaps()));

    const vk::DescriptorImageInfo image_info{
      .sampler = *sampler_,
      .imageView = image_.view(
        {.type = vk::ImageViewType::e2D,
         .format = image_.format(),
         .range =
           {.aspectMask = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel = 0,
            .levelCount = image_.mipmaps(),
            .baseArrayLayer = 0,
            .layerCount = 1}}
      ),
      .imageLayout = vk::ImageLayout::eReadOnlyOptimal
    };
    descriptor_index_ =
      graphics::context::descriptor_container().write(image_info);

    return true;
  }

  bool Texture::unload_function() { return true; }
}
