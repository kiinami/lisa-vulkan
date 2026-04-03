//
// Created by kinami on 4/2/26.
//

#include "CommandBuffer.h"

#include "graphics/context.h"

namespace lisa::graphics {
  void CommandBuffer::reset() const { buffer_.reset(); }

  void CommandBuffer::begin_onetime() const {
    buffer_.begin({.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
  }
}
