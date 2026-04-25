//
// Created by kinami on 4/25/26.
//

#ifndef LISA_VULKAN_EVENTS_H
#define LISA_VULKAN_EVENTS_H

namespace lisa::window::events {
  struct Event {};

  struct WindowEvent : Event {};

  struct WindowClose : WindowEvent {};

  struct WindowResize : WindowEvent {};

  struct KeyDown : Event {
    int code;
  };
}

#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)

#define REGISTER_EVENT_IMPL(event, function, id)           \
  namespace {                                              \
    struct CONCAT(AutoReg_, id) {                          \
      CONCAT(AutoReg_, id)() {                             \
        ::lisa::window::context::dispatcher()              \
          .sink<event>()                                   \
          .connect<&function>();                           \
      }                                                    \
    };                                                     \
    static CONCAT(AutoReg_, id) CONCAT(reg_instance_, id); \
  }

#define REGISTER_EVENT(event, function) \
  REGISTER_EVENT_IMPL(event, function, __COUNTER__)
#endif // LISA_VULKAN_EVENTS_H
