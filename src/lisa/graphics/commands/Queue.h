//
// Created by kinami on 3/31/26.
//

#ifndef LISA_VULKAN_QUEUE_H
#define LISA_VULKAN_QUEUE_H
#include "utils/common.h"

namespace lisa::graphics {
  class Queue {
  public:
    explicit Queue(vk::raii::Queue queue_) : queue_(queue_) {};
    ~Queue() = default;
  private:
    vk::raii::Queue queue_;
  };
}

#endif // LISA_VULKAN_QUEUE_H
