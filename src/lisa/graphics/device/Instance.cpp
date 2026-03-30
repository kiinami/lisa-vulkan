//
// Created by kinami on 3/24/26.
//

#include "Instance.h"

#include "lisa/graphics/vk/vk.h"
#include "lisa/utils/chk.h"
#include "lisa/utils/logging.h"

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
    const vk::DebugUtilsMessengerCreateInfoEXT debug_messenger_ci{
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
      utils::chkv(instance_.createDebugUtilsMessengerEXT(debug_messenger_ci));
  }

  Instance::Instance() {
    init_sdl();
    init_volk();

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

    volkLoadInstance(instance_);
    VULKAN_HPP_DEFAULT_DISPATCHER.init(instance_);

    if (logging::get_level() <= quill::LogLevel::Debug) add_debug_messenger();

    LOG_DEBUG(logging::logger(), "Vulkan instance initiated");
  }

  Instance::~Instance() {
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_Quit();
    instance_.destroyDebugUtilsMessengerEXT(debug_messenger_);
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
