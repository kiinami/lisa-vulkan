//
// Created by kinami on 5/2/26.
//

#ifndef LISA_VULKAN_RESOURCE_H
#define LISA_VULKAN_RESOURCE_H

#include "utils/common.h"

namespace lisa::systems::resources {

  class Resource {
  public:
    virtual ~Resource() = default;
    virtual void unload() = 0;
  };

}

#endif // LISA_VULKAN_RESOURCE_H
