//
// Created by kinami on 4/2/26.
//

#include "Buffer.h"

#include "graphics/context.h"

namespace lisa::graphics {
  Buffer::Buffer(const vk::DeviceSize size, const vk::BufferUsageFlags usage) {
    const vk::BufferCreateInfo buffer_ci{ .size = size, .usage = usage };
    buffer_ = context::allocator().create_buffer(buffer_ci);
  }
}
