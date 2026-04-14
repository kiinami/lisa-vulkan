#pragma once

#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <unordered_map>
#include <vulkan/vulkan_raii.hpp>

namespace lisa {
  typedef int8_t int8;
  typedef int32_t int32;
  typedef int64_t int64;
  typedef uint8_t uint8;
  typedef uint16_t uint16;
  typedef uint32_t uint32;
  typedef uint64_t uint64;
  typedef size_t size;

  typedef std::string str;
  template<typename T> using vector = std::vector<T>;
  template<typename K, typename V, typename H = std::hash<K>>
  using umap = std::unordered_map<K, V, H>;
  template<typename T, typename U> using pair = std::pair<T, U>;
  template<typename T, size S> using array = std::array<T, S>;
  typedef std::filesystem::path path;

  typedef glm::vec2 vec2;
  typedef glm::vec3 vec3;
  typedef glm::vec4 vec4;
  typedef glm::vec3 rgb;
  typedef glm::vec4 rgba;

  typedef glm::quat quat;

  typedef glm::mat3 mat3;
  typedef glm::mat4 mat4;
}
