//
// Created by kinami on 3/29/26.
//

#include "Resource.h"

namespace lisa::systems::resources {
  bool Resource::load() {
    loaded_ = load_function();
    if (loaded_)
      logging::trace("Loaded {} from path '{}'", type_name(), path_);
    else
      logging::error("{} in path '{}' could not be loaded", type_name(), path_);
    return loaded_;
  }
}
