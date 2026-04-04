//
// Created by kinami on 4/2/26.
//

#include "Buffer.h"

#include "graphics/context.h"

namespace lisa::graphics {
  Buffer::Buffer(const vk::DeviceSize size, const vk::BufferUsageFlags usage) {
    const vk::BufferCreateInfo buffer_ci{.size = size, .usage = usage};
    const vma::AllocationCreateInfo allocation_ci{

    };
    buffer_ = context::allocator().create_buffer(buffer_ci);
  }

  vk::DeviceAddress Buffer::address() const {
    const vk::BufferDeviceAddressInfo buffer_device_ai{
      .buffer = buffer_
    };
    return context::device()->getBufferAddress(buffer_device_ai);
  }
}
