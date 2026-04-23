//
// Created by kinami on 4/3/26.
//

#include "RenderPass.h"

#include "graphics/context.h"

namespace lisa::systems::render {
  RenderPass::~RenderPass() {
    for (const auto& [id, index] : input_indices_)
      graphics::context::descriptor_container().free(index);
  }
}
