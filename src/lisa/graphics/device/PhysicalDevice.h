//
// Created by kinami on 3/26/26.
//

#ifndef LISA_PHYSICALDEVICE_H
#define LISA_PHYSICALDEVICE_H
#include "utils/common.h"
#include <vulkan/vulkan_profiles.hpp>

namespace lisa::graphics {

  class PhysicalDevice {
  public:
    explicit PhysicalDevice(const vk::raii::PhysicalDevice& device);

    [[nodiscard]] uint8 vulkan_version() const;
    [[nodiscard]] std::string name() const;
    [[nodiscard]] bool supports_features() const;
    [[nodiscard]] bool is_discrete() const;
    [[nodiscard]] uint32 max_image_dimensions() const;
    uint32 queue_family_index() const;

  private:
    vk::raii::PhysicalDevice device_ = nullptr;
    vk::PhysicalDeviceProperties props_;
    const VpProfileProperties profile_ = {
      VP_KHR_ROADMAP_2022_NAME,
      VP_KHR_ROADMAP_2022_SPEC_VERSION
    };
  };
}

#endif
