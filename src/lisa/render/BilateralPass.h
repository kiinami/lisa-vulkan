//
// Created by kinami on 6/18/26.
//

#ifndef LISA_VULKAN_BILATERALPASS_H
#define LISA_VULKAN_BILATERALPASS_H
#pragma once

#include "ShaderPass.h"
#include "graphics/pipeline/Pipeline.h"
#include "systems/render/RenderPass.h"

namespace lisa::render {
  struct BilateralPushConstants {
    uint64 global_bda;
    DescriptorIndex shadow_idx;
    DescriptorIndex position_idx;
    DescriptorIndex normal_idx;
    float sigma_spatial;
    float sigma_depth;
    float sigma_normal;
  };

  class BilateralPass : public ShaderPass {
  public:
    static constexpr auto TYPE_ID = "postprocess::bilateral";

    explicit BilateralPass(const pugi::xml_node& node) : ShaderPass(node) {}

    void setup(
      systems::render::Rendergraph& graph, const pugi::xml_node& node
    ) override;
    void execute(const systems::render::RenderContext& ctx) override;

  private:
    uptr<graphics::Pipeline> pipeline_;
    float sigma_spatial_ = 5.0f;
    float sigma_depth_ = 0.5f;
    float sigma_normal_ = 8.0f;
  };

}

#endif // LISA_VULKAN_BILATERALPASS_H
