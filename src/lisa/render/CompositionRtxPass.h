//
// Created by kinami on 6/18/26.
//

#ifndef LISA_VULKAN_COMPOSITIONRTXPASS_H
#define LISA_VULKAN_COMPOSITIONRTXPASS_H
#pragma once

#include "ShaderPass.h"
#include "graphics/pipeline/Pipeline.h"
#include "systems/render/RenderPass.h"

namespace lisa::render {
  struct CompositionRtxPushConstants {
    uint64 global_bda;
    DescriptorIndex albedo_idx;
    DescriptorIndex normal_idx;
    DescriptorIndex position_idx;
    DescriptorIndex material_idx;
    DescriptorIndex ssao_idx;
    DescriptorIndex shadow_idx;
  };

  class CompositionRtxPass : public ShaderPass {
  public:
    static constexpr auto TYPE_ID = "deferred::composition_rtx";

    explicit CompositionRtxPass(const pugi::xml_node& node) : ShaderPass(node) {}

    void setup(
      systems::render::Rendergraph& graph, const pugi::xml_node& node
    ) override;
    void execute(const systems::render::RenderContext& ctx) override;

  private:
    uptr<graphics::Pipeline> pipeline_;
  };

}

#endif // LISA_VULKAN_COMPOSITIONRTXPASS_H
