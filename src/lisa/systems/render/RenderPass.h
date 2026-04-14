//
// Created by kinami on 4/3/26.
//

#ifndef LISA_VULKAN_RENDERPASS_H
#define LISA_VULKAN_RENDERPASS_H

#include "ShaderData.h"
#include "graphics/buffer/Buffer.h"
#include "graphics/commands/CommandBuffer.h"
#include "graphics/pipeline/Pipeline.h"
#include "scene/Scene.h"
#include "systems/resources/ResourceManager.h"
#include "utils/common.h"

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
      const scene::Scene& scene;
      const graphics::CommandBuffer& cmd;
      uint32 width;
      uint32 height;
      std::unordered_map<str, vk::ImageView> image_views;
      vk::DeviceAddress global_bda = 0;
      vk::DeviceAddress object_bda = 0;

      vk::Extent2D extent = {width, height};
    };

    explicit RenderPass(const str& name) : name_(name) {}

    virtual ~RenderPass() = default;
    RenderPass(RenderPass&&) = default;
    RenderPass& operator=(RenderPass&&) = default;
    RenderPass(const RenderPass&) = default;
    RenderPass& operator=(const RenderPass&) = default;

    const vector<ResourceUsage>& inputs() const { return inputs_; }

    const vector<ResourceUsage>& outputs() const { return outputs_; }

    void add_input(const ResourceUsage& usage) { inputs_.push_back(usage); }

    void add_output(const ResourceUsage& usage) { outputs_.push_back(usage); }

    virtual void render(const RenderPassInput& in) = 0;

  protected:
    str name_;
    vector<ResourceUsage> inputs_;
    vector<ResourceUsage> outputs_;
    std::unique_ptr<graphics::Pipeline> pipeline_;
  };

}

#endif // LISA_VULKAN_RENDERPASS_H
