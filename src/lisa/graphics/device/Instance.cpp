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
    std::vector extensions(sdl_extensions, sdl_extensions + count);
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
    constexpr vk::DebugUtilsMessengerCreateInfoEXT debug_messenger_ci{
      .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                         vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
                         vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                         vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
      .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                     vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                     vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
      .pfnUserCallback = logging::vulkanDebugCallback
    };

    debug_messenger_ =
      instance_.createDebugUtilsMessengerEXT(debug_messenger_ci);
  }

  Instance::Instance(const vk::raii::Context& ctx) {
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

    instance_ = ctx.createInstance(instance_ci, nullptr);

    if (logging::get_level() <= quill::LogLevel::Debug) add_debug_messenger();

    LOG_DEBUG(logging::logger(), "Vulkan instance initiated");
  }

  Instance::~Instance() {
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_Quit();
  }

  std::vector<PhysicalDevice> Instance::physical_devices() const {
    auto devices = instance_.enumeratePhysicalDevices();

    std::vector<PhysicalDevice> wrapped;
    wrapped.reserve(devices.size());

    std::ranges::transform(
      devices,
      std::back_inserter(wrapped),
      [](const vk::raii::PhysicalDevice& d) {
        return PhysicalDevice(d);
      }
    );

    return wrapped;
  }

  PhysicalDevice Instance::pick_physical_device() {
    const auto devices = physical_devices();
    if (devices.empty())
      logging::abort("Failed to find GPUs with Vulkan support");

    std::multimap<int, PhysicalDevice> candidates;
    for (const auto& pd : devices) {
      uint32 score = 0;

      if (!pd.supports_features()) continue;

      if (pd.is_discrete()) score += 1000;
      score += pd.max_image_dimensions();

      candidates.insert(std::make_pair(score, pd));
    }

    if (!candidates.empty() && candidates.rbegin()->first > 0)
      return candidates.rbegin()->second;

    logging::abort("Failed to find a suitable GPU");
  }
}
