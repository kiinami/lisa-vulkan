//
// Created by kinami on 3/31/26.
//

#ifndef LISA_VULKAN_LOGICALDEVICE_H
#define LISA_VULKAN_LOGICALDEVICE_H
#include "graphics/commands/CommandBuffer.h"
#include "utils/common.h"

#include <vulkan/vulkan_raii.hpp>

namespace lisa::graphics {
  class LogicalDevice : public VkObject<vk::raii::Device> {
  public:
    explicit LogicalDevice(const vk::raii::PhysicalDevice& physical_device);

    ~LogicalDevice();

    const vk::raii::Queue& queue() const { return queue_; }

    const vk::raii::CommandPool& command_pool() { return command_pool_; }

    vk::raii::ImageView
      create_image_view(const vk::ImageViewCreateInfo& view_ci) const;

    CommandBuffer cmd_buffer() const;
    void submit_cmd_buffer_with_fence(const CommandBuffer& cmd_buffer) const;

  private:
    vk::raii::Queue queue_ = nullptr;
    vk::raii::CommandPool command_pool_ = nullptr;
    vector<vk::raii::CommandBuffer> command_buffers_;
  };
}

#endif // LISA_VULKAN_LOGICALDEVICE_H
