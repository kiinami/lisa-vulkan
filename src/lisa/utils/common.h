#pragma once

#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <unordered_map>
#include <unordered_set>
#include <vulkan/vulkan_raii.hpp>

namespace lisa {
  // Numbers
  using int8 = int8_t;
  using int32 = int32_t;
  using int64 = int64_t;
  using uint8 = uint8_t;
  using uint16 = uint16_t;
  using uint32 = uint32_t;
  using uint64 = uint64_t;
  using size = size_t;

  // Strings
  using str = std::string;
  using path = std::filesystem::path;

  // Collections
  template<typename T> using vector = std::vector<T>;
  template<typename K, typename V, typename H = std::hash<K>>
  using umap = std::unordered_map<K, V, H>;
  template<typename T, typename U> using pair = std::pair<T, U>;
  template<typename T, size S> using array = std::array<T, S>;
  template<typename T> using uset = std::unordered_set<T>;

  // Pointers
  template<typename T> using uptr = std::unique_ptr<T>;
  template<typename T> using sptr = std::shared_ptr<T>;
  template<typename T> using wptr = std::weak_ptr<T>;

  // Optionals
  template<typename T> using optional = std::optional<T>;
  using std::nullopt;

  // Maths
  using vec2 = glm::vec2;
  using vec3 = glm::vec3;
  using vec4 = glm::vec4;
  using rgb = glm::vec3;
  using rgba = glm::vec4;
  using quat = glm::quat;
  using mat3 = glm::mat3;
  using mat4 = glm::mat4;
}
