//
// Created by kinami on 4/25/26.
//

#include "events.h"

#include "context.h"
#include "utils/logging.h"

namespace lisa::window::events {
  void on_window_close(const WindowClose& event) {
    logging::debug("Closing window");
  }

  REGISTER_EVENT(WindowClose, on_window_close);
}
