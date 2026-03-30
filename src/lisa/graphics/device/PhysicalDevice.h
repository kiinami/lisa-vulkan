//
// Created by kinami on 3/26/26.
//

#ifndef LISA_PHYSICALDEVICE_H
#define LISA_PHYSICALDEVICE_H
#include "lisa/utils/defines.h"
#include <vulkan/vulkan_profiles.hpp>

namespace lisa::graphics {

  class PhysicalDevice {
  public:
    explicit PhysicalDevice(vk::PhysicalDevice device);

    uint vulkan_version() const;
    std::string name() const;
    bool supports_features() const;

  private:
    vk::PhysicalDevice device_;
    vk::PhysicalDeviceProperties2 props_;
    const VpProfileProperties profile_ = {
      VP_KHR_ROADMAP_2022_NAME,
      VP_KHR_ROADMAP_2022_SPEC_VERSION
    };
  };
}

#endif
