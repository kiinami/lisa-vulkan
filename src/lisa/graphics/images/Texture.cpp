//
// Created by kinami on 4/2/26.
//

#include "Texture.h"

#include "graphics/buffer/Buffer.h"
#include "graphics/context.h"
#include "utils/chk.h"

#include <ktx.h>
#include <ktxvulkan.h>

namespace lisa::graphics {
  Texture Texture::load_ktx(
    const std::filesystem::path& filename, const CommandBuffer& cmd_buffer
  ) {
    ktxTexture* texture;
    ktxTexture_CreateFromNamedFile(
      filename.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &texture
    );

    const auto format = ImageFormat(ktxTexture_GetVkFormat(texture));
    auto image = Image(
      format,
      vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
      {texture->baseWidth, texture->baseHeight, 1},
      vk::ImageType::e2D,
      texture->numLevels,
      {.usage = vma::MemoryUsage::eAuto}
    );

    {
      auto buffer =
        Buffer(texture->dataSize, vk::BufferUsageFlagBits::eTransferSrc);

      cmd_buffer->begin(
        {.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit}
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

      std::vector<vk::BufferImageCopy> copy_regions;
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
        buffer, image, vk::ImageLayout::eTransferDstOptimal, copy_regions
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
    }

    const vk::SamplerCreateInfo sampler_ci{
      .magFilter = vk::Filter::eLinear,
      .minFilter = vk::Filter::eLinear,
      .mipmapMode = vk::SamplerMipmapMode::eLinear,
      .anisotropyEnable = vk::True,
      .maxAnisotropy = 8.0f,
      .maxLod = static_cast<float>(texture->numLevels)
    };
    auto sampler = context::device()->createSampler(sampler_ci);

    ktxTexture_Destroy(texture);

    return Texture(std::move(image), std::move(sampler));
  }

  vk::DescriptorImageInfo Texture::descriptor() {
    return {
      .sampler = sampler_,
      .imageView = image_.view(
        {.type = vk::ImageViewType::e2D,
         .format = image_.format(),
         .range =
           {.aspectMask = vk::ImageAspectFlagBits::eColor,
            .levelCount = image_.mipmaps(),
            .layerCount = 1}}
      ),
      .imageLayout = vk::ImageLayout::eReadOnlyOptimal
    };
  }
}
