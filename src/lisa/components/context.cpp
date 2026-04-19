//
// Created by kinami on 4/6/26.
//

#include "context.h"

#include "utils/logging.h"

namespace lisa::components::context {
  namespace {
    uptr<systems::ecs::Registry> registry_;
  }

  void init() {
    registry_ = std::make_unique<systems::ecs::Registry>();
    logging::debug("Component context initialized");
  }

  void destroy() {
    registry_.reset();
    logging::debug("Component context destroyed");
  }

  systems::ecs::Registry& registry() { return *registry_;};
}
