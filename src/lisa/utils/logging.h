//
// Created by kinami on 3/24/26.
//

#ifndef LISA_LOGGING_H
#define LISA_LOGGING_H
#pragma once

#include "utils/common.h"

#include <spdlog/spdlog.h>
#include <vulkan/vulkan_profiles.hpp>
#include <vulkan/vulkan_raii.hpp>

template<>
struct std::formatter<std::filesystem::path, char> :
  std::formatter<std::string> {
  auto format(const std::filesystem::path& p, std::format_context& ctx) const {
    return std::formatter<std::string>::format(p.generic_string(), ctx);
  }
};

namespace lisa::logging {
  void init(const str& level);
  void set_level(const str& level);
  spdlog::level::level_enum get_level();
  bool debug_enabled();
  VKAPI_ATTR vk::Bool32 VKAPI_CALL vulkanDebugCallback(
    vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
    vk::DebugUtilsMessageTypeFlagsEXT type,
    const vk::DebugUtilsMessengerCallbackDataEXT* callbackData,
    void* userData
  );

  template<typename... Args>
  void trace(spdlog::format_string_t<Args...> fmt, Args&&... args) {
    spdlog::trace(fmt, std::forward<Args>(args)...);
  }

  template<typename... Args>
  void debug(spdlog::format_string_t<Args...> fmt, Args&&... args) {
    spdlog::debug(fmt, std::forward<Args>(args)...);
  }

  template<typename... Args>
  void info(spdlog::format_string_t<Args...> fmt, Args&&... args) {
    spdlog::info(fmt, std::forward<Args>(args)...);
  }

  template<typename... Args>
  void warning(spdlog::format_string_t<Args...> fmt, Args&&... args) {
    spdlog::warn(fmt, std::forward<Args>(args)...);
  }

  template<typename... Args>
  void error(spdlog::format_string_t<Args...> fmt, Args&&... args) {
    spdlog::error(fmt, std::forward<Args>(args)...);
  }

  template<typename... Args>
  void critical(spdlog::format_string_t<Args...> fmt, Args&&... args) {
    spdlog::critical(fmt, std::forward<Args>(args)...);
  }

  template<typename... Args>
  [[noreturn]]
  void abort(spdlog::format_string_t<Args...> fmt, Args&&... args) {
    spdlog::critical(fmt, std::forward<Args>(args)...);
    throw std::runtime_error("Program aborted because of a critical error");
  }

  namespace detail {
    struct Part {
      str value;
      bool is_bracket;
    };

    inline Part format_part(const str& s) { return {s, false}; }

    inline Part format_part(const char* s) { return {str(s), false}; }

    inline Part format_part(const int i) {
      return {"[" + std::to_string(i) + "]", true};
    }

    inline Part format_part(const size i) {
      return {"[" + std::to_string(i) + "]", true};
    }

    inline Part format_part(const path& p) {
      return {"[" + p.filename().generic_string() + "]", true};
    }
  }

  template<typename... Args> str genid(Args&&... args) {
    str result;
    bool last_was_bracket = false;
    bool is_first = true;

    auto append = [&](const auto& arg) {
      auto part = detail::format_part(arg);
      if (!is_first && !part.is_bracket) result += "::";
      result += part.value;
      last_was_bracket = part.is_bracket;
      is_first = false;
    };

    (append(std::forward<Args>(args)), ...);
    return result;
  }
}

#endif
