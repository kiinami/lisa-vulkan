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
    struct ResourceUsage {
      str id;
      vk::ImageLayout layout;
      vk::AccessFlags access;
      vk::PipelineStageFlags stage;
      vk::ImageAspectFlags aspect;
    };

    RenderPass() = default;
    ~RenderPass() = default;

    std::function<void(const graphics::CommandBuffer&)> render_function;

    const vector<ResourceUsage>& inputs() const { return inputs_; }

    const vector<ResourceUsage>& outputs() const { return outputs_; }

  private:
    str name;
    vector<ResourceUsage> inputs_;
    vector<ResourceUsage> outputs_;
  };

}

#endif // LISA_VULKAN_RENDERPASS_H
