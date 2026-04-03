//
// Created by kinami on 4/3/26.
//

#ifndef LISA_VULKAN_RENDERER_H
#define LISA_VULKAN_RENDERER_H

#include "RenderPass.h"
#include "RenderResource.h"
#include "graphics/sync/Fence.h"
#include "utils/common.h"

#include <vulkan/vulkan_raii.hpp>

namespace lisa::systems::render {

  class Renderer {
  public:
    Renderer();
    ~Renderer();

    void render();

  private:
    graphics::Fence fence_;
    graphics::CommandBuffer cmd_buffer_;
  };

}

#endif // LISA_VULKAN_RENDERER_H
