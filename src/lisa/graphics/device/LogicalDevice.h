//
// Created by kinami on 3/31/26.
//

#ifndef LISA_VULKAN_LOGICALDEVICE_H
#define LISA_VULKAN_LOGICALDEVICE_H
#include "graphics/commands/CommandBuffer.h"

#include <vulkan/vulkan_raii.hpp>

namespace lisa::graphics {
  class LogicalDevice {
  public:
    explicit LogicalDevice(const vk::raii::PhysicalDevice& physical_device);
    ~LogicalDevice();

    vk::raii::Device* operator->() { return &device_; }

    const vk::raii::Device* operator->() const { return &device_; }

    operator const vk::raii::Device&() const { return device_; }

    const vk::raii::Queue& queue() { return queue_; }

    const vk::raii::CommandPool& command_pool() { return command_pool_; }

    vk::raii::ImageView
      create_image_view(const vk::ImageViewCreateInfo& view_ci) const;

    CommandBuffer cmd_buffer() const;
    void submit_cmd_buffer_with_fence(const CommandBuffer& cmd_buffer) const;

  private:
    vk::raii::Device device_ = nullptr;
    vk::raii::Queue queue_ = nullptr;
    vk::raii::CommandPool command_pool_ = nullptr;
    std::vector<vk::raii::CommandBuffer> command_buffers_;
  };
}

#endif // LISA_VULKAN_LOGICALDEVICE_H
