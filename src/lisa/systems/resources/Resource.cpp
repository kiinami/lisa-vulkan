//
// Created by kinami on 3/29/26.
//

#include "Resource.h"

#include "utils/path.h"

namespace lisa::systems::resources {
  bool Resource::load() {
    loaded_ = load_function();
    const auto fp = utils::pstr(path_);
    if (loaded_)
      logging::trace("Loaded {} from path '{}'", type_name(), fp.c_str());
    else
      logging::error(
        "{} in path '{}' could not be loaded", type_name(), fp.c_str()
      );
    return loaded_;
  }
}
