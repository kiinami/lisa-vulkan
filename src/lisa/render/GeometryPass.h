//
// Created by kinami on 4/19/26.
//

#ifndef LISA_VULKAN_GEOMETRYPASS_H
#define LISA_VULKAN_GEOMETRYPASS_H
#pragma once
#include "ShaderPass.h"
#include "graphics/pipeline/Pipeline.h"
#include "systems/render/RenderPass.h"

namespace lisa::render {

  class GeometryPass : public ShaderPass {
  public:
    static constexpr auto TYPE_ID = "deferred::geometry";

    explicit GeometryPass(const pugi::xml_node& node) : ShaderPass(node) {}

    ~GeometryPass() override = default;

    void setup(
      systems::render::Rendergraph& graph, const pugi::xml_node& node
    ) override;
    void execute(const systems::render::RenderContext& ctx) override;

  private:
    uptr<graphics::Pipeline> pipeline_;
  };

}

#endif // LISA_VULKAN_GEOMETRYPASS_H
