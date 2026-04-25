//
// Created by kinami on 4/25/26.
//

#ifndef LISA_VULKAN_CONTEXT_H
#define LISA_VULKAN_CONTEXT_H

#include "utils/macros.h"

#include <functional>

namespace lisa::update::context {
  void init();
  void destroy();
  void tick();
  void register_update(const std::function<void(float)>& fn);
}

#define REGISTER_UPDATE(function) REGISTER_UPDATE_IMPL(function, __COUNTER__)

#define REGISTER_UPDATE_IMPL(function, id)                              \
  namespace {                                                           \
    struct CONCAT(AutoRegUpdate_, id) {                                 \
      CONCAT(AutoRegUpdate_, id)() {                                    \
        ::lisa::update::context::register_update(&function);            \
      }                                                                 \
    };                                                                  \
    static CONCAT(AutoRegUpdate_, id) CONCAT(reg_update_instance_, id); \
  }
#endif // LISA_VULKAN_CONTEXT_H
