//
// Created by kinami on 4/2/26.
//

#ifndef LISA_VULKAN_COMMANDBUFFER_H
#define LISA_VULKAN_COMMANDBUFFER_H

#include "utils/common.h"

#include <any>
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

    template<typename T> void keep_alive(T&& object) const {
      dependencies_.emplace_back(
        std::make_shared<std::decay_t<T>>(std::forward<T>(object))
      );
    }

    void reset() const;
    void begin_onetime() const;

    void begin_region(
      const str& name, const rgba& color = {1.0, 1.0, 1.0, 1.0}
    ) const;
    void end_region() const;

  private:
    vk::raii::CommandBuffer buffer_ = nullptr;
    mutable vector<std::shared_ptr<void>> dependencies_;
  };

}

#endif // LISA_VULKAN_COMMANDBUFFER_H
