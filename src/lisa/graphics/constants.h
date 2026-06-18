//
// Created by kinami on 4/2/26.
//

#ifndef LISA_VULKAN_GRAPHICS_CONSTANTS_H
#define LISA_VULKAN_GRAPHICS_CONSTANTS_H
#include "build.h"
#include "utils/common.h"

#include <vulkan/vulkan_profiles.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace lisa::graphics::constants {
  static inline const vector<const char*> VALIDATION_LAYERS =
    build::debug ? vector{"VK_LAYER_KHRONOS_validation"}
                 : vector<const char*>{};

  static constexpr VpProfileProperties PROFILE = {
    VP_LISA_BASE_NAME, VP_LISA_BASE_SPEC_VERSION
  };
  static constexpr auto API_VERSION = VP_LISA_BASE_MIN_API_VERSION;

  static constexpr auto APPLICATION_NAME = "lisa";

  inline VpCapabilities capabilities() {
    static VpCapabilities caps = [] {
      VpCapabilities capabilities = VK_NULL_HANDLE;

      constexpr VpCapabilitiesCreateInfo createInfo{
        .flags = VP_PROFILE_CREATE_STATIC_BIT,
        .apiVersion = vk::ApiVersion13,
        .pVulkanFunctions = nullptr
      };

      vpCreateCapabilities(&createInfo, nullptr, &capabilities);

      return capabilities;
    }();
    return caps;
  }

}

#endif // LISA_VULKAN_GRAPHICS_CONSTANTS_H
