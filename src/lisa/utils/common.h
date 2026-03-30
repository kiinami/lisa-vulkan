#pragma once
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

namespace lisa {
  typedef int8_t int8;
  typedef int32_t int32;
  typedef int64_t int64;
  typedef uint8_t uint8;
  typedef uint32_t uint32;
  typedef uint64_t uint64;

  typedef glm::vec2 Vector2f;
  typedef glm::vec3 Vector3f;
  typedef glm::vec4 Vector4f;

  typedef glm::quat Quaternion;

  typedef glm::mat3 Matrix3f;
  typedef glm::mat4 Matrix4f;
}