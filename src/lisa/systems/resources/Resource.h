//
// Created by kinami on 5/2/26.
//

#ifndef LISA_VULKAN_RESOURCE_H
#define LISA_VULKAN_RESOURCE_H

#include "graphics/commands/CommandBuffer.h"
#include "utils/common.h"

#include <typeindex>

namespace lisa::systems::resources {

  class Resource {
  public:
    str id;

    explicit Resource(const str& id) : id(id) {}

    virtual ~Resource() = default;
    virtual void unload() = 0;
  };

  struct ResourceSpecBase {
    str id;

    explicit ResourceSpecBase(const str& id) : id(id) {}

    virtual ~ResourceSpecBase() = default;
    virtual uptr<Resource> load(const graphics::CommandBuffer& cmdb) = 0;
    virtual std::type_index resource_type() const = 0;
  };

  template<class R>
    requires std::derived_from<R, Resource>
  struct ResourceSpec : ResourceSpecBase {
    using ResourceSpecBase::ResourceSpecBase;

    virtual R load_resource(const graphics::CommandBuffer& cmdb) = 0;

    uptr<Resource> load(const graphics::CommandBuffer& cmdb) override {
      return std::make_unique<R>(load_resource(cmdb));
    }

    std::type_index resource_type() const override {
      return std::type_index(typeid(R));
    }
  };
}

#endif // LISA_VULKAN_RESOURCE_H
