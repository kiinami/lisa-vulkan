//
// Created by kinami on 3/26/26.
//

#ifndef LISA_PHYSICALDEVICE_H
#define LISA_PHYSICALDEVICE_H
#include "graphics/vk/VkObject.h"
#include "utils/common.h"

#include <vulkan/vulkan_profiles.hpp>

namespace lisa::graphics {

  class PhysicalDevice : public VkObject<vk::raii::PhysicalDevice> {
  public:
    explicit PhysicalDevice(const vk::raii::PhysicalDevice& device);

    uint8 vulkan_version() const;
    str name() const;
    bool supports_profile() const;
    bool is_discrete() const;
    uint32 max_image_dimensions() const;
    uint32 queue_family_index() const;
    vk::SurfaceCapabilitiesKHR
      surface_capabilities(const vk::raii::SurfaceKHR& surface) const;

  private:
    vk::PhysicalDeviceProperties props_;
  };
}

#endif
