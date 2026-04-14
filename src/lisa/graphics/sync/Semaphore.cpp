//
// Created by kinami on 4/4/26.
//

#include "Semaphore.h"

#include "graphics/context.h"

namespace lisa::graphics {
  Semaphore::Semaphore(const vk::SemaphoreCreateFlags flags) :
    semaphore_(nullptr) {
    vk::SemaphoreCreateInfo create_info{.flags = flags};
    semaphore_ = context::device()->createSemaphore(create_info);
  }
}
