//
// Created by kinami on 4/3/26.
//

#include "Texture.h"

#include "graphics/buffer/Buffer.h"
#include "graphics/context.h"
#include "graphics/descriptors/DescriptorContainer.h"
#include "utils/logging.h"
#include "utils/path.h"

#include <cstring>
#include <ktxvulkan.h>
#include <stb_image.h>
#include <tinyexr.h>

namespace lisa::resources {
  Texture::Texture(
    const str& id,
    const path& filepath,
    const graphics::CommandBuffer& cmdb,
    const int channel
  ) :
    Resource(id) {
    const auto ext = filepath.extension().string();
    if (ext == ".exr") {
      image_ = load_exr(id, filepath, cmdb, channel);
    } else {
      const auto bytes = utils::read_file(filepath);

      if (ext == ".jpg" || ext == ".jpeg" || ext == ".png")
        image_ = load_jpg(id, bytes, cmdb, channel);
      else if (ext == ".ktx" || ext == ".ktx2")
        image_ = load_ktx(id, bytes, cmdb, channel);
      else {
        logging::error("Unsupported texture format: {}", ext);
        return;
      }
    }

    setup();
  }

  Texture::Texture(
    const str& id,
    const vector<std::byte>& data,
    const fastgltf::MimeType mime,
    const graphics::CommandBuffer& cmdb,
    const int channel
  ) :
    Resource(id) {
    if (mime == fastgltf::MimeType::KTX2)
      image_ = load_ktx(id, data, cmdb, channel);
    else if (
      mime == fastgltf::MimeType::JPEG || mime == fastgltf::MimeType::PNG
    )
      image_ = load_jpg(id, data, cmdb, channel);
    else
      throw std::runtime_error("Unsupported texture format");

    setup();
  }

  void Texture::setup() {
    sampler_ = graphics::Sampler(
      logging::genid(id, "sampler"), static_cast<float>(image_.mipmaps())
    );

    const vk::DescriptorImageInfo image_info{
      .sampler = sampler_.handle(),
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

  graphics::Image Texture::load_ktx(
    const str& id,
    const vector<std::byte>& data,
    const graphics::CommandBuffer& cmdb,
    int channel
  ) {
    ktxTexture2* texture;

    auto result = ktxTexture2_CreateFromMemory(
      reinterpret_cast<const ktx_uint8_t*>(data.data()),
      data.size(),
      KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
      &texture
    );
    if (result != KTX_SUCCESS) {
      logging::error("Failed to load KTX texture: {}", ktxErrorString(result));
      return {};
    }

    if (channel >= 0 && channel <= 3) {
      logging::error(
        "Extracting a single channel from KTX textures is not supported"
      );
      ktxTexture2_Destroy(texture);
      return {};
    }

    if (ktxTexture2_NeedsTranscoding(texture)) {
      static auto target_format = KTX_TTF_BC7_RGBA;

      auto result = ktxTexture2_TranscodeBasis(texture, target_format, 0);
      if (result != KTX_SUCCESS) {
        logging::error(
          "Failed to transcode KTX texture: {}", ktxErrorString(result)
        );
      }
    }

    auto transfer_buffer = graphics::Buffer(
      logging::genid(id, "transfer[ktx]"),
      texture->dataSize,
      vk::BufferUsageFlagBits::eTransferSrc,
      {.flags = vma::AllocationCreateFlagBits::eMapped |
                vma::AllocationCreateFlagBits::eHostAccessSequentialWrite,
       .usage = vma::MemoryUsage::eAuto}
    );
    std::memcpy(
      transfer_buffer.mapped_data(), texture->pData, texture->dataSize
    );

    const auto format = graphics::ImageFormat(ktxTexture2_GetVkFormat(texture));

    auto image = graphics::Image(
      logging::genid(id, "[ktx]"),
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
    for (uint32 i = 0; i < texture->numLevels; i++) {
      ktx_size_t offset = 0;
      ktxTexture2_GetImageOffset(texture, i, 0, 0, &offset);

      const vk::BufferImageCopy buffer_image_copy{
        .bufferOffset = offset,
        .imageSubresource =
          {.aspectMask = vk::ImageAspectFlagBits::eColor,
           .mipLevel = i,
           .layerCount = 1},
        .imageExtent = {
          .width = std::max(1u, texture->baseWidth >> i),
          .height = std::max(1u, texture->baseHeight >> i),
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

    ktxTexture2_Destroy(texture);

    cmdb.keep_alive(std::move(transfer_buffer));

    return image;
  }

  graphics::Image Texture::load_jpg(
    const str& id,
    const vector<std::byte>& data,
    const graphics::CommandBuffer& cmdb,
    const int channel
  ) {
    int w, h, channels;

    stbi_uc* pixels = stbi_load_from_memory(
      reinterpret_cast<const stbi_uc*>(data.data()),
      data.size(),
      &w,
      &h,
      &channels,
      STBI_rgb_alpha
    );

    if (!pixels) {
      logging::error("Failed to load STB image: {}", stbi_failure_reason());
      return {};
    }

    const bool extract_channel = channel >= 0 && channel <= 3;

    const auto width = static_cast<uint32>(w);
    const auto height = static_cast<uint32>(h);

    const uint32 mip_levels =
      static_cast<uint32>(
        std::log2(static_cast<float>(std::max(width, height)))
      ) +
      1;

    graphics::Image image;

    if (!extract_channel) {
      const size buffer_size = width * height * 4;

      image = graphics::Image::from_data(
        logging::genid(id, "[jpg]"),
        pixels,
        buffer_size,
        {static_cast<float>(width), static_cast<float>(height), 1.0f},
        graphics::ImageFormat(vk::Format::eR8G8B8A8Unorm),
        vk::ImageUsageFlagBits::eSampled,
        cmdb,
        mip_levels
      );
    } else {
      std::vector<stbi_uc> single_channel(width * height);

      for (size_t i = 0; i < width * height; ++i)
        single_channel[i] = pixels[i * 4 + channel];

      image = graphics::Image::from_data(
        logging::genid(id, "[jpg,channel=" + std::to_string(channel) + "]"),
        single_channel.data(),
        single_channel.size(),
        {static_cast<float>(width), static_cast<float>(height), 1.0f},
        graphics::ImageFormat(vk::Format::eR8Unorm),
        vk::ImageUsageFlagBits::eSampled,
        cmdb,
        mip_levels
      );
    }

    stbi_image_free(pixels);

    return image;
  }

  graphics::Image Texture::load_exr(
    const str& id,
    const path& filepath,
    const graphics::CommandBuffer& cmdb,
    const int channel
  ) {
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

      logging::abort("Failed to load EXR: {}", filepath.string());
    }

    const bool extract_channel = channel >= 0 && channel <= 3;

    const size row_size = static_cast<size>(w) * 4 * sizeof(float);

    vector<float> temp(static_cast<size>(w) * 4);

    for (int y = 0; y < h / 2; ++y) {
      const size top_row = static_cast<size>(y) * static_cast<size>(w) * 4;

      const size bottom_row =
        static_cast<size>(h - 1 - y) * static_cast<size>(w) * 4;

      std::memcpy(temp.data(), out_rgba + top_row, row_size);
      std::memcpy(out_rgba + top_row, out_rgba + bottom_row, row_size);
      std::memcpy(out_rgba + bottom_row, temp.data(), row_size);
    }

    const uint32 width = static_cast<uint32>(w);
    const uint32 height = static_cast<uint32>(h);

    const uint32 mip_levels =
      static_cast<uint32>(
        std::log2(static_cast<float>(std::max(width, height)))
      ) +
      1;

    graphics::Image image;

    if (!extract_channel) {
      const size buffer_size = static_cast<size>(width) *
                               static_cast<size>(height) *
                               4 *
                               sizeof(float);

      image = graphics::Image::from_data(
        logging::genid(id, "[exr,channel=" + std::to_string(channel) + "]"),
        out_rgba,
        buffer_size,
        {static_cast<float>(width), static_cast<float>(height), 1.0f},
        graphics::ImageFormat(vk::Format::eR32G32B32A32Sfloat),
        vk::ImageUsageFlagBits::eSampled,
        cmdb,
        mip_levels
      );
    } else {
      std::vector<float> single_channel(
        static_cast<size>(width) * static_cast<size>(height)
      );

      for (size i = 0; i < static_cast<size>(width) * static_cast<size>(height);
           ++i) {
        single_channel[i] = out_rgba[i * 4 + channel];
      }

      image = graphics::Image::from_data(
        id + "::image[exr,single_channel]",
        single_channel.data(),
        single_channel.size() * sizeof(float),
        {static_cast<float>(width), static_cast<float>(height), 1.0f},
        graphics::ImageFormat(vk::Format::eR32Sfloat),
        vk::ImageUsageFlagBits::eSampled,
        cmdb,
        mip_levels
      );
    }

    free(out_rgba);

    return image;
  }

}
