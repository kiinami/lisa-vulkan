//
// Created by kinami on 3/24/26.
//

#include "logging.h"

#include <spdlog/sinks/stdout_color_sinks-inl.h>
#include <spdlog/spdlog.h>

namespace lisa::logging {
  namespace {
    auto main_logger_ = spdlog::stdout_color_mt("lisa");
    auto vulkan_logger_ = spdlog::stdout_color_mt("vulkan");
  }

  void init(const str& level) {
    spdlog::set_default_logger(main_logger_);
    set_level(level);
    spdlog::set_pattern("[%H:%M:%S] [%^---%L---%$] [%n] %v");
  }

  spdlog::level::level_enum get_level() { return spdlog::get_level(); }

  void set_level(const str& level) {
    if (level == "trace")
      spdlog::set_level(spdlog::level::trace);
    else if (level == "debug")
      spdlog::set_level(spdlog::level::debug);
    else if (level == "info")
      spdlog::set_level(spdlog::level::info);
    else if (level == "warning")
      spdlog::set_level(spdlog::level::warn);
    else if (level == "error")
      spdlog::set_level(spdlog::level::err);
    else if (level == "critical")
      spdlog::set_level(spdlog::level::critical);
  }

  bool debug_enabled() { return get_level() <= spdlog::level::debug; }

  VKAPI_ATTR vk::Bool32 VKAPI_CALL vulkanDebugCallback(
    vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
    vk::DebugUtilsMessageTypeFlagsEXT type,
    const vk::DebugUtilsMessengerCallbackDataEXT* callbackData,
    void* userData
  ) {
    const char* msg = callbackData->pMessage;

    if (severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
      vulkan_logger_->error(msg);
    else if (severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
      vulkan_logger_->warn(msg);
    else if (severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo)
      vulkan_logger_->debug(msg);
    else
      vulkan_logger_->trace(msg);

    return VK_FALSE;
  }
}
