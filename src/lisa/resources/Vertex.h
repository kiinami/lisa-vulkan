//
// Created by kinami on 4/2/26.
//

#ifndef LISA_VULKAN_VERTEX_H
#define LISA_VULKAN_VERTEX_H
#include "utils/common.h"

namespace lisa::resources {
  struct Vertex {
    vec3 pos;
    vec3 normal;
    vec2 uv;
  };
}

#endif // LISA_VULKAN_VERTEX_H
