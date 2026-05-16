//
// Created by kinami on 5/15/26.
//

#ifndef LISA_VULKAN_SHADOWPASS_H
#define LISA_VULKAN_SHADOWPASS_H
#pragma once

#include "ShaderPass.h"
#include "graphics/pipeline/Pipeline.h"
#include "systems/render/RenderPass.h"

namespace lisa::render {

  class ShadowPass : public ShaderPass {
  public:
    static constexpr auto TYPE_ID = "preprocess::shadow";

    explicit ShadowPass(const pugi::xml_node& node) : ShaderPass(node) {}

    void setup(
      systems::render::Rendergraph& graph, const pugi::xml_node& node
    ) override;
    void execute(const systems::render::RenderContext& ctx) override;

  private:
    uptr<graphics::Pipeline> pipeline_;
  };

}

#endif // LISA_VULKAN_SHADOWPASS_H
