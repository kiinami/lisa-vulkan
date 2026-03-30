//
// Created by kinami on 3/24/26.
//

#include "graphics.h"

#include "utils/logging.h"

namespace lisa::graphics {
  namespace {
    Instance* instance_ = nullptr;
    PhysicalDevice* physical_device_ = nullptr;
  }

  void init_device(Instance& instance, int index)
  {
    instance_ = &instance;
    auto physical_devices = instance_->physical_devices();

    if (index >= physical_devices.size())
      LOG_CRITICAL(logging::logger(), "Device index outside bounds");

    physical_device_ = &physical_devices[index];

    if (!physical_device_->supports_features())
      LOG_CRITICAL(logging::logger(), "Device not supported");

    LOG_INFO(logging::logger(), "GPU selected: {}", physical_device_->name());
  }

  void destroy_device() {}

  vk::Instance instance() { return instance_->vk_instance(); }
}
