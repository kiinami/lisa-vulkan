//
// Created by kinami on 4/3/26.
//

#ifndef LISA_VULKAN_RENDERPASS_H
#define LISA_VULKAN_RENDERPASS_H

#include "graphics/commands/CommandBuffer.h"
#include "utils/common.h"

#include <functional>

namespace lisa::systems::render {

  class RenderPass {
  public:
    RenderPass() = default;
    ~RenderPass() = default;

    std::function<void(const graphics::CommandBuffer&)> render_function;

    const vector<str>& inputs() const { return inputs_; }

    const vector<str>& outputs() const { return outputs_; }

  private:
    str name;
    vector<str> inputs_;
    vector<str> outputs_;
  };

}

#endif // LISA_VULKAN_RENDERPASS_H
