//
// Created by kinami on 4/2/26.
//

#include "CommandBuffer.h"

#include "graphics/context.h"
#include "utils/logging.h"

namespace lisa::graphics {
  void CommandBuffer::reset() const {
    buffer_.reset();
    dependencies_.clear();
  }

  void CommandBuffer::begin_onetime() const {
    buffer_.begin({.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
  }

  void CommandBuffer::begin_region(const str& name, const rgba& color) const {
#ifdef DEBUG
    vk::DebugUtilsLabelEXT label{.pLabelName = name.c_str()};
    for (size i = 0; i < 4; i++)
      label.color[i] = color[i];

    buffer_.beginDebugUtilsLabelEXT(label);
    logging::trace("Started section {}", name);
#endif
  }

  void CommandBuffer::end_region() const {
#ifdef DEBUG
    buffer_.endDebugUtilsLabelEXT();
    logging::trace("Ended section");
#endif
  }
}
