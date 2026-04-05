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

    static vector<vk::VertexInputAttributeDescription>
      attribute_descriptions(const uint32 binding) {
      const vk::VertexInputAttributeDescription pos_attribute = {
        .location = 0,
        .binding = binding,
        .format = vk::Format::eR32G32B32Sfloat
      };
      const vk::VertexInputAttributeDescription normal_attribute = {
        .location = 1,
        .binding = binding,
        .format = vk::Format::eR32G32B32Sfloat,
        .offset = offsetof(Vertex, normal)
      };
      const vk::VertexInputAttributeDescription uv_attribute = {
        .location = 2,
        .binding = binding,
        .format = vk::Format::eR32G32Sfloat,
        .offset = offsetof(Vertex, uv)
      };
      return {pos_attribute, normal_attribute, uv_attribute};
    }
  };
}

#endif // LISA_VULKAN_VERTEX_H
