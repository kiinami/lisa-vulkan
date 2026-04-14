//
// Created by kinami on 4/14/26.
//

#include "DescriptorAllocator.h"

#include "utils/logging.h"

namespace lisa::graphics {
  uint32 DescriptorAllocator::allocate() {
    std::lock_guard lock(mutex_);

    if (!free_indices_.empty()) {
      const auto id = free_indices_.front();
      free_indices_.pop();
      return id;
    }

    if (next_id_ >= max_index_) logging::abort("Descriptor container full!");
    return next_id_++;
  }

  void DescriptorAllocator::free(const uint32 id) {
    std::lock_guard lock(mutex_);
    free_indices_.push(id);
  }
}
