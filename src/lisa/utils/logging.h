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
    inline str format_part(const str& s) { return s; }

    inline str format_part(const char* s) { return str(s); }

    inline str format_part(const int i) { return std::to_string(i); }

    inline str format_part(const std::filesystem::path& p) {
      return "[" + p.filename().generic_string() + "]";
    }
  }

  template<typename... Args> str genid(Args&&... args) {
    str result;
    bool is_first = true;

    auto append = [&](const auto& arg) {
      if (!is_first) result += "::";
      result += detail::format_part(arg);
      is_first = false;
    };

    (append(std::forward<Args>(args)), ...);

    return result;
  }
}

#endif
