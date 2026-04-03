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

    operator const vk::raii::PhysicalDevice&() const { return device_; }

    operator const vk::PhysicalDevice&() const { return *device_; }

    [[nodiscard]]
    uint8 vulkan_version() const;
    [[nodiscard]]
    str name() const;
    [[nodiscard]]
    bool supports_profile() const;
    [[nodiscard]]
    bool is_discrete() const;
    [[nodiscard]]
    uint32 max_image_dimensions() const;
    uint32 queue_family_index() const;
    vk::SurfaceCapabilitiesKHR
      surface_capabilities(const vk::raii::SurfaceKHR& surface) const;

  private:
    vk::raii::PhysicalDevice device_ = nullptr;
    vk::PhysicalDeviceProperties props_;
  };
}

#endif
