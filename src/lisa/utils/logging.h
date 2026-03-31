//
// Created by kinami on 3/24/26.
//

#ifndef LISA_LOGGING_H
#define LISA_LOGGING_H
#pragma once

#include <quill/Logger.h>
#include <string>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_profiles.hpp>

namespace lisa::logging {
  void init();
  void set_level(const std::string& level);
  quill::LogLevel get_level();
  quill::Logger* logger();
  VKAPI_ATTR vk::Bool32 VKAPI_CALL vulkanDebugCallback(
    vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
    vk::DebugUtilsMessageTypeFlagsEXT type,
    const vk::DebugUtilsMessengerCallbackDataEXT* callbackData,
    void* userData
  );
  void abort(const std::string& msg);
}

#endif
