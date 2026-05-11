//
// Created by kinami on 4/3/26.
//

#include "Fence.h"

#include "graphics/context.h"
#include "utils/chk.h"

namespace lisa::graphics {
  Fence::Fence() {
    const vk::FenceCreateInfo create_info{
      .flags = vk::FenceCreateFlagBits::eSignaled
    };
    set(context::device()->createFence(create_info));
  }

  void Fence::wait() const {
    utils::chk(
      context::device()->waitForFences(handle(), vk::True, UINT64_MAX)
    );
  }

  void Fence::reset() const { context::device()->resetFences(handle()); }
}
