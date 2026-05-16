//
// Created by kinami on 5/15/26.
//

#ifndef LISA_VULKAN_CONSTANTS_H
#define LISA_VULKAN_CONSTANTS_H

#include "utils/common.h"

namespace lisa::constants {
  static constexpr uint32 MAX_FRAMES_IN_FLIGHT = 2;
  static constexpr size MAX_OBJECTS = 1024;
  static constexpr size MAX_POINT_LIGHTS = 1024;
  static constexpr size MAX_DIR_LIGHTS = 1024;
  static constexpr size MAX_AMBIENT_LIGHTS = 1024;
  static constexpr size MAX_POINT_LIGHT_SHADOWS = 4;
  static constexpr size MAX_DIR_LIGHT_SHADOWS = 4;
}

#endif // LISA_VULKAN_CONSTANTS_H
