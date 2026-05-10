//
// Created by kinami on 3/31/26.
//

#include "LogicalDevice.h"

#include "graphics/constants.h"
#include "graphics/context.h"
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

    vector device_extensions = {
      vk::KHRSwapchainExtensionName, vk::KHRShaderDrawParametersExtensionName
    };

    VkPhysicalDeviceVulkan11Features vulkan11_features{
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
      .pNext = nullptr,
      .shaderDrawParameters = VK_TRUE
    };

    VkDeviceCreateInfo vk_device_ci{
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .pNext = &vulkan11_features,
      .queueCreateInfoCount = 1,
      .pQueueCreateInfos = &device_queue_ci,
      .enabledExtensionCount = static_cast<uint32>(device_extensions.size()),
      .ppEnabledExtensionNames = device_extensions.data(),
    };

    const VpDeviceCreateInfo device_ci{
      .pCreateInfo = &vk_device_ci,
      .enabledFullProfileCount = 1,
      .pEnabledFullProfiles = &constants::PROFILE,
    };
    VkDevice dev;
    utils::chk(vpCreateDevice(
      constants::capabilities(), *physical_device, &device_ci, nullptr, &dev
    ));
    set(vk::raii::Device{physical_device, vk::Device{dev}});

    logging::debug("Logical device created");

    queue_ = object_.getQueue(queue_index, 0);

    const vk::CommandPoolCreateInfo pool_ci{
      .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
      .queueFamilyIndex = queue_index
    };
    command_pool_ = object_.createCommandPool(pool_ci);

    const vk::CommandBufferAllocateInfo command_buffers_ai{
      .commandPool = command_pool_,
      .commandBufferCount = constants::MAX_FRAMES_IN_FLIGHT
    };
    command_buffers_ = object_.allocateCommandBuffers(command_buffers_ai);
  }

  LogicalDevice::~LogicalDevice() { object_.waitIdle(); }

  vk::raii::ImageView LogicalDevice::create_image_view(
    const vk::ImageViewCreateInfo& view_ci
  ) const {
    return object_.createImageView(view_ci);
  }

  CommandBuffer LogicalDevice::cmd_buffer() const {
    return CommandBuffer(
      std::move(object_.allocateCommandBuffers(
        {.commandPool = command_pool_, .commandBufferCount = 1}
      )[0])
    );
  }

  void LogicalDevice::submit_cmd_buffer_with_fence(
    const CommandBuffer& cmd_buffer
  ) const {
    auto fence = object_.createFence({});
    const vk::SubmitInfo submit_info{
      .commandBufferCount = 1, .pCommandBuffers = &cmd_buffer.handle()
    };
    queue_.submit(submit_info, fence);
    utils::chk(object_.waitForFences({fence}, vk::True, UINT64_MAX));
  }
}
