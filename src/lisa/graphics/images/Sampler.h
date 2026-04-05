//
// Created by kinami on 4/3/26.
//

#ifndef LISA_VULKAN_SAMPLER_H
#define LISA_VULKAN_SAMPLER_H
#include "graphics/context.h"
#include "utils/common.h"

#include <vulkan/vulkan_raii.hpp>

namespace lisa::graphics {

  class Sampler {
  public:
    Sampler() = default;

    Sampler(
      const float max_lod,
      const vk::Filter mag_filter = vk::Filter::eLinear,
      const vk::Filter min_filter = vk::Filter::eLinear,
      const vk::SamplerMipmapMode mipmap_mode = vk::SamplerMipmapMode::eLinear,
      const bool anisotropic = true,
      const float max_anisotropy = 8.0f
    ) :
      sampler_(
        context::device()->createSampler(
          {.magFilter = mag_filter,
           .minFilter = min_filter,
           .mipmapMode = mipmap_mode,
           .anisotropyEnable = anisotropic,
           .maxAnisotropy = max_anisotropy,
           .maxLod = max_lod}
        )
      ) {}

    ~Sampler() = default;

    operator const vk::raii::Sampler&() { return sampler_; }

    vk::Sampler operator*() const { return *sampler_; }

    Sampler(const Sampler&) = delete;
    Sampler& operator=(const Sampler&) = delete;

    Sampler(Sampler&&) noexcept = default;
    Sampler& operator=(Sampler&&) noexcept = default;

  private:
    vk::raii::Sampler sampler_ = nullptr;
  };

}

#endif // LISA_VULKAN_SAMPLER_H
