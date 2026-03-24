//
// Created by kinami on 3/24/26.
//

#include "graphics.h"

namespace lisa::graphics {
  namespace {
    Instance* vk_instance = nullptr;
  }

  void init_device(Instance& instance) { vk_instance = &instance; }

  void destroy_device() {}
}
