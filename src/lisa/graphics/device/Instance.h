//
// Created by kinami on 3/24/26.
//

#ifndef LISA_INSTANCE_H
#define LISA_INSTANCE_H
#include "PhysicalDevice.h"

#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_profiles.hpp>
#include "utils/common.h"

namespace lisa::graphics {
  class Instance {
  public:
    explicit Instance(const vk::raii::Context& ctx);
    ~Instance();

    PhysicalDevice pick_physical_device() const;

    operator const vk::raii::Instance&() const { return instance_; }

    operator const vk::Instance&() const { return *instance_; }

    operator VkInstance() const { return *instance_; }

  private:
    vk::raii::Instance instance_ = nullptr;
    vk::raii::DebugUtilsMessengerEXT debug_messenger_ = nullptr;
    bool supports_profile();
    static vector<const char*> get_instance_extensions();
    static vector<const char*> get_validation_layers();
    void add_debug_messenger();
    vector<PhysicalDevice> physical_devices() const;
  };
}

#endif
