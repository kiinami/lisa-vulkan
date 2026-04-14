//
// Created by kinami on 4/14/26.
//

#ifndef LISA_VULKAN_DESCRIPTORALLOCATOR_H
#define LISA_VULKAN_DESCRIPTORALLOCATOR_H
#include "utils/common.h"

#include <queue>

using DescriptorIndex = uint32_t;

namespace lisa::graphics {

  class DescriptorAllocator {
  public:
    explicit DescriptorAllocator(const uint32 max_index) :
      max_index_(max_index) {}

    ~DescriptorAllocator() = default;

    DescriptorIndex allocate();
    void free(DescriptorIndex id);

  private:
    const DescriptorIndex max_index_;
    DescriptorIndex next_id_ = 0;
    std::queue<DescriptorIndex> free_indices_;
    std::mutex mutex_;
  };

}

#endif // LISA_VULKAN_DESCRIPTORALLOCATOR_H
