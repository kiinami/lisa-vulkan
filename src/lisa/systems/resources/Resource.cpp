//
// Created by kinami on 3/29/26.
//

#include "Resource.h"

namespace lisa::systems::resources {
  bool Resource::load() {
    loaded_ = load_function();
    if (loaded_)
      logging::trace("Loaded {} with ID '{}'", type_name(), id_);
    else
      logging::error("{} with ID '{}' could not be loaded", type_name(), id_);
    return loaded_;
  }
}
