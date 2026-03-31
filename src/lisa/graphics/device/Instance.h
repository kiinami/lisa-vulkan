//
// Created by kinami on 3/24/26.
//

#ifndef LISA_INSTANCE_H
#define LISA_INSTANCE_H
#include "PhysicalDevice.h"

#include <vulkan/vulkan_raii.hpp>

static const std::vector<const char*> VLAYERS = {};
static const std::vector VLAYERS_DEBUG = { "VK_LAYER_KHRONOS_validation" };

namespace lisa::graphics {
  class Instance {
  public:
    explicit Instance(const vk::raii::Context& ctx);
    ~Instance();

    PhysicalDevice pick_physical_device();

    vk::raii::Instance& vk_instance() { return instance_; }

  private:
    vk::raii::Instance instance_ = nullptr;
    vk::raii::DebugUtilsMessengerEXT debug_messenger_ = nullptr;

    static std::vector<const char*> get_instance_extensions();
    static std::vector<const char*> get_validation_layers();
    void add_debug_messenger();
    std::vector<PhysicalDevice> physical_devices() const;
  };
}

#endif
