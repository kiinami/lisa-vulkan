//
// Created by kinami on 4/3/26.
//

#ifndef LISA_VULKAN_RENDERRESOURCE_H
#define LISA_VULKAN_RENDERRESOURCE_H

#include "graphics/images/Image.h"
#include "utils/common.h"

namespace lisa::systems::render {

  class RenderResource {
  public:
    RenderResource(
      const str& name,
      graphics::ImageFormat format,
      const vec3& size,
      vk::ImageUsageFlags usage
    );
    ~RenderResource() = default;

    RenderResource(const RenderResource&) = delete;
    RenderResource& operator=(const RenderResource&) = delete;

    RenderResource(RenderResource&&) noexcept = default;
    RenderResource& operator=(RenderResource&&) noexcept = default;

    const str& name() const { return name_; }

    graphics::Image& image() { return image_; }

    const graphics::Image& image() const { return image_; }

  private:
    str name_;
    graphics::Image image_;
  };

}

#endif // LISA_VULKAN_RENDERRESOURCE_H
