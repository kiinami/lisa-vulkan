//
// Created by kinami on 4/3/26.
//

#ifndef LISA_VULKAN_FENCE_H
#define LISA_VULKAN_FENCE_H

#include "utils/common.h"

#include <vulkan/vulkan_raii.hpp>

namespace lisa::graphics {

  class Fence {
  public:
    Fence();
    ~Fence() = default;

    vk::raii::Fence* operator->() { return &fence_; }

    const vk::raii::Fence* operator->() const { return &fence_; }

    operator const vk::raii::Fence&() { return fence_; }

    void wait() const;

    void reset() const;

  private:
    vk::raii::Fence fence_ = nullptr;
  };

}

#endif // LISA_VULKAN_FENCE_H
