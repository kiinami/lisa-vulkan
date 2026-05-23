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

    inline static const path shaders_path = {LISA_SHADER_DIR};
}

#endif // LISA_VULKAN_BUILD_H
