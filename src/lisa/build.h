//
// Created by kinami on 5/6/26.
//

#ifndef LISA_VULKAN_BUILD_H
#define LISA_VULKAN_BUILD_H

namespace lisa::build {
  inline constexpr bool debug =
#ifdef DEBUG
    true;
#else
    false;
#endif
}

#endif // LISA_VULKAN_BUILD_H
