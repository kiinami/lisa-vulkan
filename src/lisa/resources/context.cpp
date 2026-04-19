//
// Created by kinami on 4/5/26.
//

#include "context.h"

namespace lisa::resources::context {
  namespace {
    uptr<systems::resources::ResourceManager> manager_;
  }

  void init() {
    manager_ = std::make_unique<systems::resources::ResourceManager>();
    logging::debug("Resources context initialized");
  }

  void destroy() {
    manager_.reset();
    logging::debug("Resources context destroyed");
  }

  systems::resources::ResourceManager& manager() { return *manager_; }
}
