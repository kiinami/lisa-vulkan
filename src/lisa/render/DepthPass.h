//
// Created by kinami on 4/22/26.
//

#ifndef LISA_VULKAN_DEPTHPASS_H
#define LISA_VULKAN_DEPTHPASS_H
#pragma once
#include "graphics/pipeline/Pipeline.h"
#include "systems/render/RenderPass.h"
#include "systems/resources/ResourceHandle.h"

namespace lisa::render {

  class DepthPass : public systems::render::RenderPass {
  public:
    static constexpr auto TYPE_ID = "depth";

    explicit DepthPass(const pugi::xml_node& node) : RenderPass(node) {}

    ~DepthPass() override = default;

    void setup(
      systems::render::Rendergraph& graph, const pugi::xml_node& node
    ) override;
    void execute(const systems::render::RenderContext& ctx) override;

  private:
    systems::resources::ResourceHandle<resources::Shader> shader_;
    uptr<graphics::Pipeline> pipeline_;
  };

}

#endif // LISA_VULKAN_DEPTHPASS_H
