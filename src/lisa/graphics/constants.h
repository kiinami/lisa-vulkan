//
// Created by kinami on 4/2/26.
//

#ifndef LISA_VULKAN_CONSTANTS_H
#define LISA_VULKAN_CONSTANTS_H
#include "utils/common.h"

#include <vulkan/vulkan_raii.hpp>

namespace lisa::graphics::constants {
  static constexpr vector<const char*> VALIDATION_LAYERS = {};
  static const vector VALIDATION_LAYERS_DEBUG = {"VK_LAYER_KHRONOS_validation"};

  static constexpr VpProfileProperties PROFILE = {
    VP_KHR_ROADMAP_2022_NAME, VP_KHR_ROADMAP_2022_SPEC_VERSION
  };
  static constexpr auto API_VERSION = VP_KHR_ROADMAP_2022_MIN_API_VERSION;

  static constexpr auto APPLICATION_NAME = "lisa";

  inline VpCapabilities capabilities() {
    static VpCapabilities caps = [] {
      VpCapabilities capabilities = VK_NULL_HANDLE;

      constexpr VpCapabilitiesCreateInfo createInfo{
        .flags = VP_PROFILE_CREATE_STATIC_BIT,
        .apiVersion = vk::ApiVersion11,
        .pVulkanFunctions = nullptr
      };

      vpCreateCapabilities(&createInfo, nullptr, &capabilities);

      return capabilities;
    }();
    return caps;
  }

  static constexpr uint32 MAX_FRAMES_IN_FLIGHT = 2;
}

#endif // LISA_VULKAN_CONSTANTS_H
