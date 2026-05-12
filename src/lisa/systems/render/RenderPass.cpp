//
// Created by kinami on 4/3/26.
//

#include "RenderPass.h"

#include "graphics/context.h"
#include "graphics/descriptors/DescriptorContainer.h"

namespace lisa::systems::render {
  RenderPass::~RenderPass() {
    for (const auto& index : input_indices_ | std::views::values)
      graphics::context::descriptor_container().free(index);
  }
}
