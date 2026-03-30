//
// Created by kinami on 3/24/26.
//

#include "Instance.h"

#include "graphics/vk/vk.h"
#include "quill/LogMacros.h"
#include "utils/chk.h"
#include "utils/logging.h"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_vulkan.h>

namespace lisa::graphics {
  std::vector<const char*> Instance::get_instance_extensions() {
    uint32_t count = 0;
    const char* const* sdl_extensions =
      SDL_Vulkan_GetInstanceExtensions(&count);
    std::vector<const char*> extensions(sdl_extensions, sdl_extensions + count);
    if (logging::get_level() <= quill::LogLevel::Debug)
      extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    return extensions;
  }

  std::vector<const char*> Instance::get_validation_layers() {
    const auto& vlayers =
      logging::get_level() <= quill::LogLevel::Debug ? VLAYERS_DEBUG : VLAYERS;

    std::vector<const char*> return_layers;
    const auto available_layers = vk::enumerateInstanceLayerProperties();
    for (auto layer : vlayers) {
      bool found = false;

      for (const auto& available_layer : available_layers) {
        if (strcmp(layer, available_layer.layerName) == 0) {
          found = true;
          return_layers.push_back(layer);
          break;
        }
      }

      if (!found)
        LOG_ERROR(logging::logger(), "Validation layer not found: {}", layer);
    }

    return return_layers;
  }

  void Instance::add_debug_messenger() {
    vk::DebugUtilsMessengerCreateInfoEXT debug_messenger_ci{
      .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                         vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
                         vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                         vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
      .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                     vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                     vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
      .pfnUserCallback = logging::vulkanDebugCallback
    };

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
      instance_, "vkCreateDebugUtilsMessengerEXT"
    );
    if (func != nullptr) {
      VkDebugUtilsMessengerEXT messenger;
      VkDebugUtilsMessengerCreateInfoEXT c_info = debug_messenger_ci;
      if (func(instance_, &c_info, nullptr, &messenger) == VK_SUCCESS)
        debug_messenger_ = messenger;
    }
  }

  Instance::Instance() {
    init_sdl();

    vk::ApplicationInfo app_info{ .pApplicationName = "lisa",
                                  .pEngineName = "lisa",
                                  .apiVersion = VK_API_VERSION_1_3 };

    auto extensions = get_instance_extensions();
    auto layers = get_validation_layers();

    const vk::InstanceCreateInfo instance_ci{
      .pApplicationInfo = &app_info,
      .enabledLayerCount = static_cast<uint>(layers.size()),
      .ppEnabledLayerNames = layers.data(),
      .enabledExtensionCount = static_cast<uint>(extensions.size()),
      .ppEnabledExtensionNames = extensions.data(),
    };

    utils::chk(vk::createInstance(&instance_ci, nullptr, &instance_));

    if (logging::get_level() <= quill::LogLevel::Debug) add_debug_messenger();

    LOG_DEBUG(logging::logger(), "Vulkan instance initiated");
  }

  Instance::~Instance() {
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_Quit();

    if (debug_messenger_) {
      auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
        instance_, "vkDestroyDebugUtilsMessengerEXT"
      );
      if (func != nullptr) func(instance_, debug_messenger_, nullptr);
    }

    instance_.destroy();
  }

  std::vector<PhysicalDevice> Instance::physical_devices() const {
    auto devices = instance_.enumeratePhysicalDevices();

    std::vector<PhysicalDevice> wrapped;
    wrapped.reserve(devices.size());

    std::ranges::transform(
      devices, std::back_inserter(wrapped), [](const vk::PhysicalDevice& d) {
        return PhysicalDevice(d);
      }
    );

    return wrapped;
  }
}
