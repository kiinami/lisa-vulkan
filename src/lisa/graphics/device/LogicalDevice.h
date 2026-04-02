//
// Created by kinami on 3/31/26.
//

#ifndef LISA_VULKAN_LOGICALDEVICE_H
#define LISA_VULKAN_LOGICALDEVICE_H
#include "graphics/commands/Queue.h"

#include <vulkan/vulkan_raii.hpp>

namespace lisa::graphics {
  class LogicalDevice {
  public:
    explicit LogicalDevice(const vk::raii::PhysicalDevice& physical_device);
    ~LogicalDevice();

    operator const vk::raii::Device&() const { return device_; }

    const vk::raii::Queue& queue() { return queue_; }

    const vk::raii::CommandPool& command_pool() { return command_pool_; }

    vk::raii::ImageView
      create_image_view(const vk::ImageViewCreateInfo& view_ci) const;

  private:
    vk::raii::Device device_ = nullptr;
    vk::raii::Queue queue_ = nullptr;
    vk::raii::CommandPool command_pool_ = nullptr;
    std::vector<vk::raii::CommandBuffer> command_buffers_;
  };
}

#endif // LISA_VULKAN_LOGICALDEVICE_H
