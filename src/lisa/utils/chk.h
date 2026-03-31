//
// Created by kinami on 3/24/26.
//

#ifndef LISA_CHK_H
#define LISA_CHK_H

#include "logging.h"

#include <SDL3/SDL_error.h>

namespace lisa::utils {
  inline void chk(const vk::Result result) {
    if (result != vk::Result::eSuccess)
      logging::error("Vulkan call returned an error");
  }

  template<typename RV> auto chkv(RV&& rv) {
    if constexpr (std::is_class_v<std::decay_t<RV>> && requires {
                    rv.result;
                    rv.value;
                  }) {
      if (rv.result != vk::Result::eSuccess)
        logging::error("Vulkan call returned an error");
      return std::forward<RV>(rv).value;
    } else {
      return std::forward<RV>(rv);
    }
  }

  inline void chk(const VkResult result) {
    if (result != VK_SUCCESS) {
      logging::error(
        "Vulkan call returned an error (VkResult: {})", static_cast<int>(result)
      );
    }
  }

  inline void chk(const bool result) {
    if (!result) logging::abort("Call returned an error: {}", SDL_GetError());
  }
}

#endif
