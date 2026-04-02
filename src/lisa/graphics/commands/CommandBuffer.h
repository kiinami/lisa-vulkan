//
// Created by kinami on 4/2/26.
//

#ifndef LISA_VULKAN_COMMANDBUFFER_H
#define LISA_VULKAN_COMMANDBUFFER_H

#include <vulkan/vulkan_raii.hpp>

namespace lisa::graphics {

  class CommandBuffer {
  public:
    CommandBuffer(vk::raii::CommandBuffer cmd_buffer) :
      buffer_(std::move(cmd_buffer)) {}

    ~CommandBuffer() = default;

    vk::raii::CommandBuffer* operator->() { return &buffer_; }

    const vk::raii::CommandBuffer* operator->() const { return &buffer_; }

    operator const vk::raii::CommandBuffer&() const { return buffer_; }

    operator vk::raii::CommandBuffer&() { return buffer_; }

    operator const vk::CommandBuffer&() const { return *buffer_; }

    operator vk::CommandBuffer() { return buffer_; }

    operator const vk::CommandBuffer*() const { return &*buffer_; }

  private:
    vk::raii::CommandBuffer buffer_ = nullptr;
  };

}

#endif // LISA_VULKAN_COMMANDBUFFER_H
