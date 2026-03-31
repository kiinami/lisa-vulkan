//
// Created by kinami on 3/31/26.
//

#include "LogicalDevice.h"

#include "graphics/context.h"
#include "utils/common.h"
#include "utils/logging.h"

namespace lisa::graphics {
  LogicalDevice::LogicalDevice(
    const vk::raii::PhysicalDevice& physical_device
  ) {
    const auto queue_family_props = physical_device.getQueueFamilyProperties();
    uint32 queue_index = ~0;

    for (uint32 i = 0; i < queue_family_props.size(); i++) {
      if (queue_family_props[i].queueFlags & vk::QueueFlagBits::eGraphics) {
        queue_index = i;
        break;
      }
    }

    if (queue_index == ~0)
      logging::abort("Could not find a suitable queue for the device");

    float queue_priority = 0.5f;
    VkDeviceQueueCreateInfo device_queue_ci{
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .queueFamilyIndex = queue_index,
      .queueCount = 1,
      .pQueuePriorities = &queue_priority
    };

    VkDeviceCreateInfo vk_device_ci{
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .queueCreateInfoCount = 1,
      .pQueueCreateInfos = &device_queue_ci,
    };

    const VpDeviceCreateInfo device_ci{ .pCreateInfo = &vk_device_ci };
    VkDevice dev;
    vpCreateDevice(*physical_device, &device_ci, nullptr, &dev);
    const auto device = vk::Device{ dev };
    device_ = vk::raii::Device{ physical_device, device };

    logging::debug("Logical device created");
  }

  LogicalDevice::~LogicalDevice() { device_.waitIdle(); }
}
