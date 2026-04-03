//
// Created by kinami on 4/3/26.
//

#ifndef LISA_VULKAN_RENDERPASS_H
#define LISA_VULKAN_RENDERPASS_H

#include "graphics/commands/CommandBuffer.h"
#include "utils/common.h"

namespace lisa::systems::render {

  class RenderPass {
  public:
    RenderPass();
    ~RenderPass();

    virtual void
      render_function(const graphics::CommandBuffer& cmd_buffer) const;

    const vector<str>& inputs() const { return inputs_; }

    const vector<str>& outputs() const { return outputs_; }

  private:
    str name;
    vector<str> inputs_;
    vector<str> outputs_;
  };

}

#endif // LISA_VULKAN_RENDERPASS_H
