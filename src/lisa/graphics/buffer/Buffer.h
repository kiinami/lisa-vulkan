//
// Created by kinami on 4/2/26.
//

#ifndef LISA_VULKAN_BUFFER_H
#define LISA_VULKAN_BUFFER_H
#include "vk_mem_alloc_raii.hpp"

#include <vulkan/vulkan_raii.hpp>

namespace lisa::graphics {

  class Buffer {
  public:
    Buffer() = default;
    Buffer(vk::DeviceSize size, vk::BufferUsageFlags usage);
    ~Buffer() = default;

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    Buffer(Buffer&&) noexcept = default;
    Buffer& operator=(Buffer&&) noexcept = default;

    operator const vma::raii::Buffer&() { return buffer_; }

    const vma::raii::Allocation& allocation() const {
      return buffer_.getAllocation();
    }

  private:
    vma::raii::Buffer buffer_ = nullptr;
  };

}

#endif // LISA_VULKAN_BUFFER_H
