//
// Created by kinami on 4/3/26.
//

#include "RenderResource.h"

namespace lisa::systems::render {
  RenderResource::RenderResource(
    const str& name,
    const graphics::ImageFormat format,
    const vec3& size,
    const vk::ImageUsageFlags usage
  ) :
    name_(name),
    image_(graphics::Image(format, usage, size)) {
    image_.view(
      {.type = vk::ImageViewType::e2D,
       .format = image_.format(),
       .range = {
         .aspectMask = format.aspect_mask(),
         .baseMipLevel = 0,
         .levelCount = 1,
         .baseArrayLayer = 0,
         .layerCount = 1
       }}
    );
  }
}
