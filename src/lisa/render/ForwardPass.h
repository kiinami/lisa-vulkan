//
// Created by kinami on 4/18/26.
//

#ifndef LISA_VULKAN_FORWARDPASS_H
#define LISA_VULKAN_FORWARDPASS_H
#include "ShaderPass.h"
#include "graphics/pipeline/Pipeline.h"
#include "systems/render/RenderPass.h"

namespace lisa::render {
  class ForwardPass : public ShaderPass {
  public:
    static constexpr auto TYPE_ID = "forward";

    explicit ForwardPass(const pugi::xml_node& node) : ShaderPass(node) {}

    ~ForwardPass() override = default;
    void setup(
      systems::render::Rendergraph& graph, const pugi::xml_node& node
    ) override;
    void execute(const systems::render::RenderContext& ctx) override;

  private:
    uptr<graphics::Pipeline> pipeline_;
  };
}
#endif
