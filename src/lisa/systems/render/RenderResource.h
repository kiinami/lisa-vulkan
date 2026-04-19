//
// Created by kinami on 4/3/26.
//

#ifndef LISA_VULKAN_RENDERRESOURCE_H
#define LISA_VULKAN_RENDERRESOURCE_H

#include "graphics/images/Image.h"
#include "utils/common.h"

#include <utility>

namespace lisa::systems::render {
  struct RenderResourceDesc {
    str type;
  };

  class RenderResource {
  public:
    RenderResource() = default;

    explicit RenderResource(str id) : id_(std::move(id)) {}

    virtual ~RenderResource() = default;

    virtual str type() const = 0;

  private:
    str id_;
  };

}

#endif // LISA_VULKAN_RENDERRESOURCE_H
