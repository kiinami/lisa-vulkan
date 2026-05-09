//
// Created by kinami on 4/2/26.
//

#include "CommandBuffer.h"

#include "build.h"
#include "graphics/context.h"
#include "utils/logging.h"

namespace lisa::graphics {
  void CommandBuffer::reset() const {
    object_.reset();
    dependencies_.clear();
  }

  void CommandBuffer::begin_onetime() const {
    object_.begin({.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
  }

  void CommandBuffer::begin_region(const str& name, const rgba& color) const {
    if constexpr (build::debug) {
      vk::DebugUtilsLabelEXT label{.pLabelName = name.c_str()};
      for (size i = 0; i < 4; i++)
        label.color[i] = color[i];

      object_.beginDebugUtilsLabelEXT(label);
      logging::trace("Started section {}", name);
    }
  }

  void CommandBuffer::end_region() const {
    if constexpr (build::debug) {
      object_.endDebugUtilsLabelEXT();
      logging::trace("Ended section");
    }
  }
}
