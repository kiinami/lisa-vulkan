//
// Created by kinami on 4/2/26.
//

#ifndef LISA_VULKAN_BUFFER_H
#define LISA_VULKAN_BUFFER_H
#include "utils/common.h"

#include <vk_mem_alloc_raii.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace lisa::graphics {

  class Buffer {
  public:
    Buffer() = default;
    Buffer(
      vk::DeviceSize size,
      vk::BufferUsageFlags usage,
      const vma::AllocationCreateInfo& allocation_ci = {}
    );
    ~Buffer() = default;

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    Buffer(Buffer&&) noexcept = default;
    Buffer& operator=(Buffer&&) noexcept = default;

    operator const vma::raii::Buffer&() const { return buffer_; }

    operator const vk::Buffer&() const { return *buffer_; }

    const vma::raii::Allocation& allocation() const {
      return buffer_.getAllocation();
    }

    static Buffer from_data(
      const void* data,
      size size,
      vk::BufferUsageFlags usage,
      const vma::AllocationCreateInfo& allocation_ci = {}
    );
    vk::DeviceAddress address() const;
    void* mapped_data() const;

  private:
    vma::raii::Buffer buffer_ = nullptr;
  };

}

#endif // LISA_VULKAN_BUFFER_H
