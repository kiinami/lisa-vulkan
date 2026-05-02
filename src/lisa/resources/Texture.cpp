//
// Created by kinami on 4/3/26.
//

#include "Texture.h"

#include "graphics/buffer/Buffer.h"
#include "graphics/context.h"
#include "utils/logging.h"
#include "utils/path.h"

#include <cstring>
#include <ktxvulkan.h>
#include <stb_image.h>
#include <tinyexr.h>

namespace lisa::resources {
  graphics::Image Texture::load_ktx(const path& filepath) {
    ktxTexture* texture;
    const auto fp = utils::pstr(filepath);
    ktxTexture_CreateFromNamedFile(
      fp.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &texture
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

  graphics::Image Texture::load_jpg(const path& filepath) {
    int w, h, channels;
    stbi_set_flip_vertically_on_load(true);
    stbi_uc* pixels = stbi_load(
      utils::pstr(filepath).c_str(), &w, &h, &channels, STBI_rgb_alpha
    );
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

    auto image = graphics::Image::from_data(
      pixels,
      buffer_size,
      {static_cast<float>(width), static_cast<float>(height), 1.0f},
      graphics::ImageFormat(vk::Format::eR8G8B8A8Unorm),
      vk::ImageUsageFlagBits::eSampled,
      mip_levels
    );

    stbi_image_free(pixels);

    return image;
  }

  graphics::Image Texture::load_exr(const path& filepath) {
    float* out_rgba = nullptr;
    int w, h;
    const char* err = nullptr;

    if (
      const auto ret =
        LoadEXR(&out_rgba, &w, &h, utils::pstr(filepath).c_str(), &err);
      ret != TINYEXR_SUCCESS
    ) {
      if (err) {
        logging::abort("Failed to load EXR: {} - {}", filepath.string(), err);
        FreeEXRErrorMessage(err);
      }
    }

    // Flip vertically: EXR has Y going up, Vulkan expects Y going down
    const int row_size = w * 4 * sizeof(float);
    std::vector<float> temp(row_size);
    for (int y = 0; y < h / 2; y++) {
      const int top_row = y * w * 4;
      const int bottom_row = (h - 1 - y) * w * 4;
      std::memcpy(temp.data(), out_rgba + top_row, row_size);
      std::memcpy(out_rgba + top_row, out_rgba + bottom_row, row_size);
      std::memcpy(out_rgba + bottom_row, temp.data(), row_size);
    }

    auto width = static_cast<uint32>(w);
    auto height = static_cast<uint32>(h);
    const auto mip_levels =
      static_cast<uint32>(
        std::log2(static_cast<float>(std::max(width, height)))
      ) +
      1;
    const auto buffer_size = width * height * 4 * sizeof(float);

    auto image = graphics::Image::from_data(
      out_rgba,
      buffer_size,
      {static_cast<float>(width), static_cast<float>(height), 1.0f},
      graphics::ImageFormat(vk::Format::eR32G32B32A32Sfloat),
      vk::ImageUsageFlagBits::eSampled,
      mip_levels
    );

    free(out_rgba);

    return image;
  }

  Texture::Texture(const path& filepath) {
    if (const auto ext = filepath.extension().string();
        ext == ".ktx" || ext == ".ktx2"
    ) {
      image_ = load_ktx(filepath);
    } else if (ext == ".jpg" || ext == ".jpeg" || ext == ".png") {
      image_ = load_jpg(filepath);
    } else if (ext == ".exr") {
      image_ = load_exr(filepath);
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
  }
}
