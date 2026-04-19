//
// Created by kinami on 4/18/26.
//

#ifndef LISA_VULKAN_UTILS_REGISTRY_H
#define LISA_VULKAN_UTILS_REGISTRY_H
#include "common.h"
#include "logging.h"

#include <functional>

namespace lisa::utils {

  template<typename T, typename... ConstructArgs> class Registry {
  public:
    using CreateFunction = std::function<uptr<T>(ConstructArgs...)>;

    static void add(const str& type_id, CreateFunction f) {
      auto& reg = registry();
      if (reg.contains(type_id))
        logging::abort("Duplicate name in registry: {}", type_id);
      reg[type_id] = std::move(f);
    }

    static uptr<T> create(const str& type_id, ConstructArgs... args) {
      auto& reg = registry();
      const auto it = reg.find(type_id);
      if (it == reg.end())
        logging::abort("Name not found in registry: {}", type_id);
      return it->second(std::forward<ConstructArgs>(args)...);
    }

  private:
    static umap<str, CreateFunction>& registry() {
      static umap<str, CreateFunction> r;
      return r;
    }
  };

}

#endif // LISA_VULKAN_REGISTRY_H
