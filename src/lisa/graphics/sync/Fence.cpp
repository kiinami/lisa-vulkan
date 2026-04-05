//
// Created by kinami on 4/3/26.
//

#include "Fence.h"

#include "graphics/context.h"
#include "utils/chk.h"

namespace lisa::graphics {
  Fence::Fence() {
    vk::FenceCreateInfo create_info{
      .flags = vk::FenceCreateFlagBits::eSignaled
    };
    fence_ = context::device()->createFence(create_info);
  }

  void Fence::wait() const {
    utils::chk(context::device()->waitForFences(*fence_, vk::True, UINT64_MAX));
  }

  void Fence::reset() const { context::device()->resetFences(*fence_); }
}
