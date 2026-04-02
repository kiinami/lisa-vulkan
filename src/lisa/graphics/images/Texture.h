//
// Created by kinami on 4/2/26.
//

#ifndef LISA_VULKAN_TEXTURE_H
#define LISA_VULKAN_TEXTURE_H
#include "Image.h"
#include "graphics/commands/CommandBuffer.h"

#include <filesystem>

namespace lisa::graphics {

  class Texture {
  public:
    static Texture load_ktx(
      const std::filesystem::path& filename, const CommandBuffer& cmd_buffer
    );

    Texture(Image image, vk::raii::Sampler sampler) :
      image_(std::move(image)),
      sampler_(std::move(sampler)) {}

  private:
    Image image_;
    vk::raii::Sampler sampler_;
  };

}

#endif // LISA_VULKAN_TEXTURE_H
