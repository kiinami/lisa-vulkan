//
// Created by kinami on 4/14/26.
//

#ifndef LISA_VULKAN_PARSER_H
#define LISA_VULKAN_PARSER_H

#include "entt/meta/meta.hpp"
#include "entt/meta/resolve.hpp"
#include "logging.h"
#include "utils/common.h"

namespace lisa::utils {

  static vector<float> extract_floats(const str& str) {
    auto sanitized = str;
    std::ranges::replace(sanitized, ',', ' ');

    std::stringstream ss(sanitized);
    vector<float> floats;
    float val;
    while (ss >> val)
      floats.push_back(val);
    return floats;
  }

  template<typename T> static T parse(const str& value) {
    if constexpr (std::is_same_v<T, float>) return std::stof(value);

    if constexpr (std::is_same_v<T, double>) return std::stod(value);

    if constexpr (std::is_same_v<T, int>) return std::stoi(value);

    if constexpr (std::is_same_v<T, bool>)
      return value == "true" || value == "1";

    if constexpr (std::is_same_v<T, str>) return value;

    if constexpr (std::is_same_v<T, path>) return path(value);

    const auto f = extract_floats(value);

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

  static entt::meta_any
    parse_value_string(const entt::meta_type& type, const str& value) {
    if (type == entt::resolve<float>()) return parse<float>(value);
    if (type == entt::resolve<int>()) return parse<int>(value);
    if (type == entt::resolve<bool>()) return parse<bool>(value);
    if (type == entt::resolve<str>()) return parse<str>(value);
    if (type == entt::resolve<path>()) return parse<path>(value);
    if (type == entt::resolve<vec3>()) return parse<vec3>(value);
    if (type == entt::resolve<rgb>()) return parse<rgb>(value);
    if (type == entt::resolve<vec4>()) return parse<vec4>(value);
    if (type == entt::resolve<rgba>()) return parse<rgba>(value);
    if (type == entt::resolve<mat3>()) return parse<mat3>(value);
    if (type == entt::resolve<mat4>()) return parse<mat4>(value);
    return {};
  }
}

#endif // LISA_VULKAN_PARSER_H
