//
// Created by kinami on 4/3/26.
//

#ifndef LISA_VULKAN_RENDERPASS_H
#define LISA_VULKAN_RENDERPASS_H

#include "ShaderData.h"
#include "graphics/buffer/Buffer.h"
#include "graphics/commands/CommandBuffer.h"
#include "graphics/pipeline/Pipeline.h"
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

    struct RenderPassInput {
      const graphics::CommandBuffer& cmd_buffer;
      uint32 width;
      uint32 height;
      vk::Extent2D extent = {width, height};
      std::unordered_map<str, vk::ImageView> image_views;
    };

    RenderPass(
      const str& name,
      const std::function<void(const RenderPassInput&)>& render_function,
      const vector<ResourceUsage>& inputs,
      const vector<ResourceUsage>& outputs
    ) :
      name_(name),
      render_function_(render_function),
      inputs_(inputs),
      outputs_(outputs) {}

    ~RenderPass() = default;
    RenderPass(RenderPass&&) = default;
    RenderPass& operator=(RenderPass&&) = default;
    RenderPass(const RenderPass&) = default;
    RenderPass& operator=(const RenderPass&) = default;

    const vector<ResourceUsage>& inputs() const { return inputs_; }

    const vector<ResourceUsage>& outputs() const { return outputs_; }

    void add_input(const ResourceUsage& usage) { inputs_.push_back(usage); }

    void add_output(const ResourceUsage& usage) { outputs_.push_back(usage); }

    void render(const RenderPassInput& render_info) const {
      return render_function_(render_info);
    }

  private:
    str name_;
    std::function<void(const RenderPassInput&)> render_function_;
    vector<ResourceUsage> inputs_;
    vector<ResourceUsage> outputs_;
  };

}

#endif // LISA_VULKAN_RENDERPASS_H
