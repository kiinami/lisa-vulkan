//
// Created by kinami on 4/17/26.
//

#ifndef LISA_VULKAN_UTILS_XML_H
#define LISA_VULKAN_UTILS_XML_H
#include "logging.h"
#include "utils/common.h"

#include <magic_enum/magic_enum.hpp>
#include <pugixml.hpp>

namespace lisa::utils::xml {
  inline vector<float> parse_float_list(const str& str) {
    auto sanitized = str;
    std::ranges::replace(sanitized, ',', ' ');

    std::stringstream ss(sanitized);
    vector<float> floats;
    float val;
    while (ss >> val)
      floats.push_back(val);
    return floats;
  }

  pugi::xml_document read(const path& filepath, const str& top_level = "");

  template<typename T> T parse(const str& value) {
    if constexpr (std::is_same_v<T, float>) return std::stof(value);

    if constexpr (std::is_same_v<T, double>) return std::stod(value);

    if constexpr (std::is_same_v<T, int>) return std::stoi(value);

    if constexpr (std::is_same_v<T, bool>)
      return value == "true" || value == "1";

    if constexpr (std::is_same_v<T, str>) return value;

    if constexpr (std::is_same_v<T, path>) return path(value);

    const auto f = parse_float_list(value);

    if constexpr (std::is_same_v<T, vec3>) {
      if (f.empty()) return vec3{0.0f};
      if (f.size() == 1) return vec3{f[0]};
      return vec3{f[0], f.size() > 1 ? f[1] : 0.0f, f.size() > 2 ? f[2] : 0.0f};
    }
    if constexpr (std::is_same_v<T, rgb>) {
      if (f.empty()) return rgb{0.0f};
      if (f.size() == 1) return rgb{f[0]};
      return rgb{f[0], f.size() > 1 ? f[1] : 0.0f, f.size() > 2 ? f[2] : 0.0f};
    }
    if constexpr (std::is_same_v<T, vec4>) {
      if (f.empty()) return vec4{0.0f};
      if (f.size() == 1) return vec4{f[0]};
      return vec4{
        f[0],
        f.size() > 1 ? f[1] : 0.0f,
        f.size() > 2 ? f[2] : 0.0f,
        f.size() > 3 ? f[3] : 0.0f
      };
    }
    if constexpr (std::is_same_v<T, rgba>) {
      if (f.empty()) return rgba{0.0f};
      if (f.size() == 1) return rgba{f[0]};
      return rgba{
        f[0],
        f.size() > 1 ? f[1] : 0.0f,
        f.size() > 2 ? f[2] : 0.0f,
        f.size() > 3 ? f[3] : 0.0f
      };
    }
    if constexpr (std::is_same_v<T, mat3>) {
      mat3 m{1.0f};
      float* ptr = glm::value_ptr(m);
      for (int i = 0; i < std::min(static_cast<int>(f.size()), 9); ++i)
        ptr[i] = f[i];
      return m;
    }
    if constexpr (std::is_same_v<T, mat4>) {
      mat4 m{1.0f};
      float* ptr = glm::value_ptr(m);
      for (int i = 0; i < std::min(static_cast<int>(f.size()), 16); ++i)
        ptr[i] = f[i];
      return m;
    }

    return T{};
  }

  template<typename T>
    requires std::is_enum_v<T>
  T parse_enum(const str& value) {
    auto v = magic_enum::enum_cast<T>(value);
    if (v.has_value()) return v.value();
    logging::abort("String '{}' could not be casted into enum value", value);
  }
}

#endif // LISA_VULKAN_UTILS_XML_H
