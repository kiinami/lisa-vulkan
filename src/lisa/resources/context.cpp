//
// Created by kinami on 4/5/26.
//

#include "context.h"

namespace lisa::resources::context {
  namespace {
    std::unique_ptr<systems::resources::ResourceManager> manager_;
  }

  void init() {
    manager_ = std::make_unique<systems::resources::ResourceManager>();
  }

  void destroy() {
    manager_.reset();
  }

  systems::resources::ResourceManager& manager() { return *manager_; }
}
