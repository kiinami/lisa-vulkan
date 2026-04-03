//
// Created by kinami on 4/3/26.
//

#include "Texture.h"

#include "constants.h"
#include "graphics/buffer/Buffer.h"

#include <ktxvulkan.h>

namespace lisa::resources {
  ktxTexture* Texture::load_from_file(const path& filepath) {
    ktxTexture* texture;
    ktxTexture_CreateFromNamedFile(
      filepath.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &texture
    );
    return texture;
  }

  void Texture::copy(ktxTexture* texture, const graphics::Image& image) {
    auto cmd_buffer = graphics::context::device().cmd_buffer();

    cmd_buffer.begin_onetime();

    auto transfer_buffer = graphics::Buffer(
      texture->dataSize, vk::BufferUsageFlagBits::eTransferSrc
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
    cmd_buffer->pipelineBarrier2(dependency_i);

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
    cmd_buffer->copyBufferToImage(
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
    cmd_buffer->pipelineBarrier2(dependency_i);

    cmd_buffer->end();
    graphics::context::device().submit_cmd_buffer_with_fence(cmd_buffer);
  }

  bool Texture::load_function() {
    const path filepath = constants::TEXTURES_PATH / (id_ + ".ktx");
    const auto texture = load_from_file(filepath);

    const auto format = graphics::ImageFormat(ktxTexture_GetVkFormat(texture));

    image_ = graphics::Image(
      format,
      vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
      {texture->baseWidth, texture->baseHeight, 1},
      vk::ImageType::e2D,
      texture->numLevels,
      vk::ImageLayout::eUndefined,
      {.usage = vma::MemoryUsage::eAuto}
    );

    copy(texture, image_);

    sampler_ = graphics::Sampler(static_cast<float>(texture->numLevels));

    ktxTexture_Destroy(texture);
    return true;
  }

  bool Texture::unload_function() { return true; }
}
