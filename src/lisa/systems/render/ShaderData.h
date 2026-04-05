//
// Created by kinami on 4/4/26.
//

#ifndef LISA_VULKAN_SHADERDATA_H
#define LISA_VULKAN_SHADERDATA_H

#include "utils/common.h"

namespace lisa::systems::render {

  struct alignas(16) GlobalViewData {
    mat4 view_projection;
    mat4 projection;
    mat4 view;
    vec4 camera_position;
  };

  struct alignas(16) ObjectData {
    mat4 model;
    vec4 color;
    uint32 texture_index;
  };

  struct PushConstants {
    uint64 global_bda;
    uint64 object_bda;
  };

}

#endif //LISA_VULKAN_SHADERDATA_H
