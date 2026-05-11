//
// Created by kinami on 4/3/26.
//

#ifndef LISA_VULKAN_SAMPLER_H
#define LISA_VULKAN_SAMPLER_H

#include "graphics/vk/NamedVkObject.h"

#include <vulkan/vulkan_raii.hpp>

namespace lisa::graphics {

  class Sampler : public NamedVkObject<vk::raii::Sampler> {
  public:
    Sampler() = default;

    explicit Sampler(
      const str& id,
      float max_lod,
      vk::Filter mag_filter = vk::Filter::eLinear,
      vk::Filter min_filter = vk::Filter::eLinear,
      vk::SamplerMipmapMode mipmap_mode = vk::SamplerMipmapMode::eLinear,
      bool anisotropic = true,
      float max_anisotropy = 8.0f
    );
  };

}

#endif // LISA_VULKAN_SAMPLER_H
