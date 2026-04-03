//
// Created by kinami on 3/24/26.
//

#include "Instance.h"

#include "graphics/constants.h"
#include "utils/chk.h"
#include "utils/logging.h"

#include <SDL3pp/SDL3pp_init.h>
#include <SDL3pp/SDL3pp_vulkan.h>

namespace lisa::graphics {
  bool Instance::supports_profile() {
    auto supported = vk::False;

    utils::chk(vpGetInstanceProfileSupport(
      constants::capabilities(), nullptr, &constants::PROFILE, &supported
    ));

    return static_cast<bool>(supported);
  }

  vector<const char*> Instance::get_instance_extensions() {
    auto sdl_extensions = SDL::Vulkan_GetInstanceExtensions();
    vector<const char*> extensions(
      sdl_extensions.begin(), sdl_extensions.end()
    );
    if (logging::debug_enabled())
      extensions.push_back(vk::EXTDebugUtilsExtensionName);
    return extensions;
  }

  vector<const char*> Instance::get_validation_layers() {
    const auto& vlayers = logging::debug_enabled()
                            ? constants::VALIDATION_LAYERS_DEBUG
                            : constants::VALIDATION_LAYERS;

    vector<const char*> return_layers;
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

      if (!found) logging::abort("Validation layer not found: {}", layer);
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
    if (!supports_profile())
      logging::abort("Profile not supported at instance level");

    vk::ApplicationInfo app_info{
      .pApplicationName = constants::APPLICATION_NAME,
      .pEngineName = constants::APPLICATION_NAME,
      .apiVersion = constants::API_VERSION
    };

    auto extensions = get_instance_extensions();
    auto layers = get_validation_layers();

    const vk::InstanceCreateInfo instance_ci{
      .pApplicationInfo = &app_info,
      .enabledLayerCount = static_cast<uint>(layers.size()),
      .ppEnabledLayerNames = layers.data(),
      .enabledExtensionCount = static_cast<uint>(extensions.size()),
      .ppEnabledExtensionNames = extensions.data(),
    };

    const VpInstanceCreateInfo vp_instance_ci{
      .pCreateInfo = instance_ci,
      .enabledFullProfileCount = 1,
      .pEnabledFullProfiles = &constants::PROFILE
    };

    VkInstance instance = VK_NULL_HANDLE;
    utils::chk(vpCreateInstance(
      constants::capabilities(), &vp_instance_ci, nullptr, &instance
    ));
    instance_ = vk::raii::Instance(ctx, instance);

    if (logging::debug_enabled()) add_debug_messenger();

    logging::debug("Vulkan instance initiated");
  }

  Instance::~Instance() {}

  vector<PhysicalDevice> Instance::physical_devices() const {
    auto devices = instance_.enumeratePhysicalDevices();

    vector<PhysicalDevice> wrapped;
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

  PhysicalDevice Instance::pick_physical_device() const {
    const auto devices = physical_devices();
    if (devices.empty())
      logging::abort("Failed to find GPUs with Vulkan support");

    std::multimap<int, PhysicalDevice> candidates;
    for (const auto& pd : devices) {
      uint32 score = 0;

      if (!pd.supports_profile()) continue;

      if (pd.is_discrete()) score += 1000;
      score += pd.max_image_dimensions();

      candidates.insert(std::make_pair(score, pd));
    }

    if (!candidates.empty() && candidates.rbegin()->first > 0) {
      auto selected = candidates.rbegin()->second;
      logging::info("Selected GPU device: '{}'", selected.name());
      return selected;
    }

    logging::abort("Failed to find a suitable GPU");
  }
}
