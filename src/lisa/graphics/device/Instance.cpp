//
// Created by kinami on 3/24/26.
//

#include "Instance.h"

#include "lisa/graphics/vk/vk.h"
#include "lisa/utils/chk.h"
#include "lisa/utils/logging.h"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_vulkan.h>
#include <quill/LogMacros.h>
#include <volk.h>

namespace lisa {
  Instance::Instance() {
    graphics::init_sdl();
    graphics::init_volk();

    VkApplicationInfo appInfo{ .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                               .pApplicationName = "lisa",
                               .pEngineName = "lisa",
                               .apiVersion = VK_API_VERSION_1_3 };

    uint32_t instanceExtensionsCount{ 0 };
    const char* const* instanceExtensions{
      SDL_Vulkan_GetInstanceExtensions(&instanceExtensionsCount)
    };

    static constexpr std::array validation_layers = {
      "VK_LAYER_KHRONOS_validation"
    };

    VkInstanceCreateInfo instanceCI{
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pApplicationInfo = &appInfo,
      .enabledExtensionCount = instanceExtensionsCount,
      .ppEnabledExtensionNames = instanceExtensions,
    };
    instanceCI.enabledLayerCount = validation_layers.size();
    instanceCI.ppEnabledLayerNames = validation_layers.data();

    utils::chk(vkCreateInstance(&instanceCI, nullptr, &instance_));

    volkLoadInstance(instance_);

    LOG_DEBUG(logging::logger(), "Vulkan instance initiated");
  }

  Instance::~Instance() {
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_Quit();
    vkDestroyInstance(instance_, nullptr);
  }
} // namespace lisa
