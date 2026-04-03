//
// Created by kinami on 3/26/26.
//

#include "PhysicalDevice.h"

#include "graphics/constants.h"
#include "graphics/context.h"
#include "utils/chk.h"
#include "utils/common.h"

namespace lisa::graphics {
  PhysicalDevice::PhysicalDevice(const vk::raii::PhysicalDevice& device) :
    device_(device) {
    props_ = device_.getProperties2().properties;
    logging::debug("Physical device with name '{}' created", name());
  }

  uint8 PhysicalDevice::vulkan_version() const { return props_.apiVersion; }

  str PhysicalDevice::name() const { return props_.deviceName; }

  bool PhysicalDevice::supports_profile() const {
    vk::Bool32 supported;
    utils::chk(vpGetPhysicalDeviceProfileSupport(
      constants::capabilities(),
      context::instance(),
      *device_,
      &constants::PROFILE,
      &supported
    ));
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

  vk::SurfaceCapabilitiesKHR PhysicalDevice::surface_capabilities(
    const vk::raii::SurfaceKHR& surface
  ) const {
    return device_.getSurfaceCapabilitiesKHR(surface);
  }
}
