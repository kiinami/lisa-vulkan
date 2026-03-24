//
// Created by kinami on 3/24/26.
//

#ifndef LISA_LOGGING_H
#define LISA_LOGGING_H
#pragma once
#include <quill/Logger.h>
#include <string>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

namespace lisa::logging {
  void init();
  void set_level(const std::string& level);
  quill::Logger* logger();
  VKAPI_ATTR VkBool32 VKAPI_CALL vulkanDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
    void* userData
  );

}

#endif
