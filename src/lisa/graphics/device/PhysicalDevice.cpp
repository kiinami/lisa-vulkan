//
// Created by kinami on 3/26/26.
//

#include "PhysicalDevice.h"

#include "graphics/graphics.h"
#include "utils/chk.h"

#include <vulkan/vulkan.hpp>

namespace lisa::graphics {
  PhysicalDevice::PhysicalDevice(const vk::PhysicalDevice device) :
    device_(device) {
    props_ = device_.getProperties2();
  }

  uint8 PhysicalDevice::vulkan_version() const {
    return props_.properties.apiVersion;
  }

  std::string PhysicalDevice::name() const {
    return props_.properties.deviceName;
  }

  bool PhysicalDevice::supports_features() const {
    vk::Bool32 supported;
    vpGetPhysicalDeviceProfileSupport(
      instance(), device_, &profile_, &supported
    );
    return supported;
  }
}
