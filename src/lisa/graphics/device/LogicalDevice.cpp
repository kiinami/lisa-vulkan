//
// Created by kinami on 3/31/26.
//

#include "LogicalDevice.h"

#include "graphics/constants.h"
#include "graphics/context.h"
#include "utils/Fence.h"
#include "utils/chk.h"
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

    const char* device_extensions[] = {vk::KHRSwapchainExtensionName};

    VkDeviceCreateInfo vk_device_ci{
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .queueCreateInfoCount = 1,
      .pQueueCreateInfos = &device_queue_ci,
      .enabledExtensionCount = 1,
      .ppEnabledExtensionNames = device_extensions,
    };

    const VpDeviceCreateInfo device_ci{
      .pCreateInfo = &vk_device_ci,
      .enabledFullProfileCount = 1,
      .pEnabledFullProfiles = &constants::PROFILE
    };
    VkDevice dev;
    lisa::utils::chk(vpCreateDevice(
      constants::capabilities(), *physical_device, &device_ci, nullptr, &dev
    ));
    device_ = vk::raii::Device{physical_device, vk::Device{dev}};

    logging::debug("Logical device created");

    queue_ = device_.getQueue(queue_index, 0);

    const vk::CommandPoolCreateInfo pool_ci{
      .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
      .queueFamilyIndex = queue_index
    };
    command_pool_ = device_.createCommandPool(pool_ci);

    const vk::CommandBufferAllocateInfo command_buffers_ai{
      .commandPool = command_pool_,
      .commandBufferCount = constants::MAX_FRAMES_IN_FLIGHT
    };
    command_buffers_ = device_.allocateCommandBuffers(command_buffers_ai);
  }

  LogicalDevice::~LogicalDevice() { device_.waitIdle(); }

  vk::raii::ImageView LogicalDevice::create_image_view(
    const vk::ImageViewCreateInfo& view_ci
  ) const {
    return device_.createImageView(view_ci);
  }

  const CommandBuffer& LogicalDevice::cmd_buffer() const {
    return CommandBuffer(
      std::move(device_.allocateCommandBuffers(
        {.commandPool = command_pool_, .commandBufferCount = 1}
      )[0])
    );
  }

  void LogicalDevice::submit_cmd_buffer_with_fence(const CommandBuffer& cmd_buffer) const {
    auto fence = device_.createFence({});
    const vk::SubmitInfo submit_info{
      .commandBufferCount = 1, .pCommandBuffers = cmd_buffer
    };
    queue_.submit(submit_info, fence);
    lisa::utils::chk(device_.waitForFences({fence}, vk::True, UINT64_MAX));
  }
}
