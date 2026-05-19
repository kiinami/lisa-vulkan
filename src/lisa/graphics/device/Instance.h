//
// Created by kinami on 3/24/26.
//

#ifndef LISA_INSTANCE_H
#define LISA_INSTANCE_H
#include "PhysicalDevice.h"
#include "graphics/vk/VkObject.h"
#include "utils/common.h"

#include <vulkan/vulkan_profiles.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace lisa::graphics {
  class Instance : public VkObject<vk::raii::Instance> {
  public:
    explicit Instance(const vk::raii::Context& ctx);

    PhysicalDevice pick_physical_device() const;

  private:
    vk::raii::DebugUtilsMessengerEXT debug_messenger_ = nullptr;

    static bool supports_profile();
    static vector<const char*> get_instance_extensions();
    static vector<const char*> get_validation_layers();
    void add_debug_messenger();
    vector<PhysicalDevice> physical_devices() const;
  };
}

#endif
