//
// Created by kinami on 4/14/26.
//

#ifndef LISA_VULKAN_RESOURCECOMPONENT_H
#define LISA_VULKAN_RESOURCECOMPONENT_H
#include "Component.h"
#include "resources/context.h"
#include "systems/resources/ResourceHandle.h"
#include "systems/resources/ResourceManager.h"
#include "utils/common.h"

namespace lisa::systems::ecs {

  template<typename T> struct ResourceComponent : Component {
    path filepath;

    str id() const { return filepath.stem(); }

    T* resource() const {
      if (!handle_) handle_ = lisa::resources::context::manager().load<T>(id());
      return handle_.get();
    }

  private:
    mutable resources::ResourceHandle<T> handle_;
  };

}

#endif // LISA_VULKAN_RESOURCECOMPONENT_H
