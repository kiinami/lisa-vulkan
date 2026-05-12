//
// Created by kinami on 4/22/26.
//

#ifndef LISA_VULKAN_DEPTHPASS_H
#define LISA_VULKAN_DEPTHPASS_H
#pragma once
#include "ShaderPass.h"
#include "graphics/pipeline/Pipeline.h"
#include "systems/render/RenderPass.h"

namespace lisa::render {

  class DepthPass : public ShaderPass {
  public:
    static constexpr auto TYPE_ID = "preprocess::depth";

    explicit DepthPass(const pugi::xml_node& node) : ShaderPass(node) {}

    ~DepthPass() override = default;

    void setup(
      systems::render::Rendergraph& graph, const pugi::xml_node& node
    ) override;
    void execute(const systems::render::RenderContext& ctx) override;

  private:
    uptr<graphics::Pipeline> pipeline_;
  };

}

#endif // LISA_VULKAN_DEPTHPASS_H
