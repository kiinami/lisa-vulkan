//
// Created by kinami on 4/4/26.
//

#include "Semaphore.h"

#include "graphics/context.h"

namespace lisa::graphics {
  Semaphore::Semaphore(const vk::SemaphoreCreateFlags flags) {
    set(context::device()->createSemaphore({.flags = flags}));
  }
}
