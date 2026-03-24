//
// Created by kinami on 3/24/26.
//

#include "logging.h"

#include <quill/Backend.h>
#include <quill/Frontend.h>
#include <quill/LogMacros.h>
#include <quill/Logger.h>
#include <quill/sinks/ConsoleSink.h>

namespace lisa::logging {
  using namespace quill;

  namespace {
    std::shared_ptr<Sink> sink_;
    Logger* logger_;
    Logger* vulkan_logger_;
  } // namespace

  void init() {
    Backend::start();
    sink_ = Frontend::create_or_get_sink<ConsoleSink>("lisa");
    logger_ = Frontend::create_or_get_logger("lisa", std::move(sink_));
    vulkan_logger_ = Frontend::create_or_get_logger("vulkan", std::move(sink_));
  }

  void set_level(const std::string& level) {
    if (level == "trace") {
      logger_->set_log_level(LogLevel::TraceL1);
      vulkan_logger_->set_log_level(LogLevel::TraceL1);
    } else if (level == "debug") {
      logger_->set_log_level(LogLevel::Debug);
      vulkan_logger_->set_log_level(LogLevel::Debug);
    } else if (level == "info") {
      logger_->set_log_level(LogLevel::Info);
      vulkan_logger_->set_log_level(LogLevel::Info);
    } else if (level == "warning") {
      logger_->set_log_level(LogLevel::Warning);
      vulkan_logger_->set_log_level(LogLevel::Warning);
    } else if (level == "error") {
      logger_->set_log_level(LogLevel::Error);
      vulkan_logger_->set_log_level(LogLevel::Error);
    } else if (level == "critical") {
      logger_->set_log_level(LogLevel::Critical);
      vulkan_logger_->set_log_level(LogLevel::Critical);
    }
  }

  Logger* logger() { return logger_; }

  VKAPI_ATTR VkBool32 VKAPI_CALL vulkanDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
    void* userData
  ) {
    const char* msg = callbackData->pMessage;

    if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
      LOG_ERROR(vulkan_logger_, "[VULKAN] {}", msg);
    else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
      LOG_WARNING(vulkan_logger_, "[VULKAN] {}", msg);
    else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
      LOG_INFO(vulkan_logger_, "[VULKAN] {}", msg);
    else
      LOG_DEBUG(vulkan_logger_, "[VULKAN] {}", msg);

    return VK_FALSE;
  }
}
