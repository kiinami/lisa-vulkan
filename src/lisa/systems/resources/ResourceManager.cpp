//
// Created by kinami on 3/29/26.
//

#include "ResourceManager.h"

#include <ranges>

namespace lisa::systems::resources {

  void ResourceManager::flush_deleted_resources() {
    for (auto& resource : dead_resources_)
      if (resource) resource->unload();
    dead_resources_.clear();
  }

  void ResourceManager::unload_all() {
    for (auto& type_resources : resources_ | std::views::values) {
      for (auto& data : type_resources | std::views::values)
        if (data.resource) data.resource->unload();
      type_resources.clear();
    }
    resources_.clear();

    flush_deleted_resources();
  }

}
