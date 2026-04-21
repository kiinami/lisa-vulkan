//
// Created by kinami on 4/3/26.
//

#ifndef LISA_VULKAN_SAMPLER_H
#define LISA_VULKAN_SAMPLER_H

#include <vulkan/vulkan_raii.hpp>

namespace lisa::graphics {

  class Sampler {
  public:
    Sampler() = default;

    explicit Sampler(
      float max_lod,
      vk::Filter mag_filter = vk::Filter::eLinear,
      vk::Filter min_filter = vk::Filter::eLinear,
      vk::SamplerMipmapMode mipmap_mode = vk::SamplerMipmapMode::eLinear,
      bool anisotropic = true,
      float max_anisotropy = 8.0f
    );

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
