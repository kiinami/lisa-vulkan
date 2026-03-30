//
// Created by kinami on 3/24/26.
//

#ifndef LISA_INSTANCE_H
#define LISA_INSTANCE_H
#include "PhysicalDevice.h"
#include "lisa/utils/defines.h"

static const std::vector<const char*> VLAYERS = {};
static const std::vector VLAYERS_DEBUG = {
  "VK_LAYER_KHRONOS_validation"
};

namespace lisa::graphics {
  class Instance {
  public:
    Instance();
    ~Instance();

    [[nodiscard]]
    vk::Instance vk_instance() const {
      return instance_;
    }

    std::vector<PhysicalDevice> physical_devices() const;

  private:
    vk::Instance instance_ = VK_NULL_HANDLE;
    vk::DebugUtilsMessengerEXT debug_messenger_ = VK_NULL_HANDLE;

    static std::vector<const char*> get_instance_extensions();
    static std::vector<const char*> get_validation_layers();
    void add_debug_messenger();
  };
}

#endif
