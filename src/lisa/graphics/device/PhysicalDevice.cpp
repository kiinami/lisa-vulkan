//
// Created by kinami on 3/26/26.
//

#include "PhysicalDevice.h"

#include "graphics/context.h"
#include "utils/chk.h"
#include "utils/common.h"

namespace lisa::graphics {
  PhysicalDevice::PhysicalDevice(const vk::raii::PhysicalDevice& device) :
    device_(device) {
    props_ = device_.getProperties2().properties;
  }

  const vk::raii::PhysicalDevice& PhysicalDevice::vk_physical_device() {
    return device_;
  }

  uint8 PhysicalDevice::vulkan_version() const { return props_.apiVersion; }

  std::string PhysicalDevice::name() const { return props_.deviceName; }

  bool PhysicalDevice::supports_features() const {
    vk::Bool32 supported;
    vpGetPhysicalDeviceProfileSupport(
      *context::vk_instance(), *device_, &profile_, &supported
    );
    return supported;
  }

  bool PhysicalDevice::is_discrete() const {
    return props_.deviceType == vk::PhysicalDeviceType::eDiscreteGpu;
  }

  uint32 PhysicalDevice::max_image_dimensions() const {
    return props_.limits.maxImageDimension2D;
  }

  uint32 PhysicalDevice::queue_family_index() const {
    auto queue_families = device_.getQueueFamilyProperties();
    const auto graphics_queue =
      std::ranges::find_if(queue_families, [](const auto& qf) {
        return (qf.queueFlags & vk::QueueFlagBits::eGraphics) !=
               static_cast<vk::QueueFlags>(0);
      });
    return static_cast<uint32>(
      std::distance(queue_families.begin(), graphics_queue)
    );
  }
}
