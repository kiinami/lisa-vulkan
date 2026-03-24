//
// Created by kinami on 3/24/26.
//

#ifndef LISA_CHK_H
#define LISA_CHK_H

#include <iostream>
#include <vulkan/vulkan_core.h>

namespace lisa::utils {
  inline void chk(const VkResult result) {
    if (result != VK_SUCCESS) {
      std::cerr << "Vulkan call returned an error (" << result << ")\n";
      exit(result);
    }
  }

  inline void chk(const bool result) {
    if (!result) {
      std::cerr << "Call returned an error\n";
      exit(result);
    }
  }
} // namespace lisa::utils

#endif // LISA_CHK_H
