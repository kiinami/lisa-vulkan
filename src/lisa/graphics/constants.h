//
// Created by kinami on 4/2/26.
//

#ifndef LISA_VULKAN_GRAPHICS_CONSTANTS_H
#define LISA_VULKAN_GRAPHICS_CONSTANTS_H
#include "build.h"
#include "utils/common.h"
#include "utils/logging.h"

#include <vulkan/vulkan_profiles.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace lisa::graphics::constants {
  static inline const vector<const char*> VALIDATION_LAYERS =
    build::debug ? vector{"VK_LAYER_KHRONOS_validation"}
                 : vector<const char*>{};

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

  inline VpProfileProperties& active_profile() {
    static VpProfileProperties profile = {
      VP_LISA_BASE_NAME, VP_LISA_BASE_SPEC_VERSION
    };
    return profile;
  }

  inline void set_active_profile(const VpProfileProperties& p) {
    logging::debug(
      "Setting Vulkan profile to {} (version {})", p.profileName, p.specVersion
    );
    active_profile() = p;
  }

  inline optional<VpProfileProperties> resolve_profile(const str& name) {
    uint32 count = 0;
    vpGetProfiles(capabilities(), &count, nullptr);
    vector<VpProfileProperties> profiles(count);
    vpGetProfiles(capabilities(), &count, profiles.data());
    static const str prefix = "VP_LISA_";
    for (const auto& p : profiles)
      if (
        str full = p.profileName;
        full.starts_with(prefix) && full.substr(prefix.size()) == name
      )
        return p;
    return nullopt;
  }

  inline uint32 active_api_version() {
    return vpGetProfileAPIVersion(capabilities(), &active_profile());
  }

}

#endif // LISA_VULKAN_GRAPHICS_CONSTANTS_H
