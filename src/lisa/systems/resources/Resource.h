//
// Created by kinami on 5/2/26.
//

#ifndef LISA_VULKAN_RESOURCE_H
#define LISA_VULKAN_RESOURCE_H

#include "graphics/commands/CommandBuffer.h"
#include "utils/common.h"

namespace lisa::systems::resources {

  class Resource {
  public:
    virtual ~Resource() = default;
    virtual void unload() = 0;
  };

  struct ResourceSpecBase {
    virtual ~ResourceSpecBase() = default;
    virtual uptr<Resource> load(const graphics::CommandBuffer& cmdb) = 0;
  };

  template<class T>
    requires std::derived_from<T, Resource>
  struct ResourceSpec : ResourceSpecBase {
    virtual T load_resource(const graphics::CommandBuffer& cmdb) = 0;

    uptr<Resource> load(const graphics::CommandBuffer& cmdb) override {
      return std::make_unique<T>(load_resource(cmdb));
    }
  };
}

#endif // LISA_VULKAN_RESOURCE_H
