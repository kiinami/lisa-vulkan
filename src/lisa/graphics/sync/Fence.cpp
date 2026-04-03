//
// Created by kinami on 4/3/26.
//

#include "Fence.h"

#include "graphics/context.h"
#include "utils/chk.h"

namespace lisa::graphics {
  Fence::Fence() { fence_ = context::device()->createFence({}); }

  void Fence::wait() const {
    utils::chk(context::device()->waitForFences(*fence_, vk::True, UINT32_MAX));
  }

  void Fence::reset() const { context::device()->resetFences(*fence_); }
}
