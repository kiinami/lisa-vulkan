//
// Created by kinami on 4/19/26.
//

#ifndef LISA_VULKAN_COMPOSITIONPASS_H
#define LISA_VULKAN_COMPOSITIONPASS_H
#pragma once
#include "graphics/images/Sampler.h"
#include "graphics/pipeline/Pipeline.h"
#include "systems/render/RenderPass.h"
#include "systems/resources/ResourceHandle.h"

namespace lisa::render {
  struct CompositionPushConstants {
    uint64 global_bda;
    uint32 albedo_idx;
    uint32 normal_idx;
    uint32 position_idx;
    uint32 material_idx;
  };

  class CompositionPass : public systems::render::RenderPass {
  public:
    static constexpr auto TYPE_ID = "deferred::CompositionPass";

    explicit CompositionPass(const pugi::xml_node& node) : RenderPass(node) {}
    ~CompositionPass() override = default;

    void setup(systems::render::Rendergraph& graph, const pugi::xml_node& node) override;
    void execute(const systems::render::RenderContext& ctx) override;
    
  private:
    systems::resources::ResourceHandle<resources::Shader> shader_;
    uptr<graphics::Pipeline> pipeline_;
    uptr<graphics::Sampler> sampler_;

    uint32 albedo_idx_;
    uint32 normal_idx_;
    uint32 position_idx_;
    uint32 material_idx_;
  };

}

#endif //LISA_VULKAN_COMPOSITIONPASS_H
