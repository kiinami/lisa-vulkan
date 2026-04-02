#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <glm/glm.hpp>

namespace lisa {
  typedef int8_t int8;
  typedef int32_t int32;
  typedef int64_t int64;
  typedef uint8_t uint8;
  typedef uint16_t uint16;
  typedef uint32_t uint32;
  typedef uint64_t uint64;

  typedef glm::vec2 vec2;
  typedef glm::vec3 vec3;
  typedef glm::vec4 vec4;

  typedef glm::quat Quaternion;

  typedef glm::mat3 mat3;
  typedef glm::mat4 mat4;
}