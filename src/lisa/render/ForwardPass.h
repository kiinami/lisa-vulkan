//
// Created by kinami on 4/18/26.
//

#ifndef LISA_VULKAN_FORWARDPASS_H
#define LISA_VULKAN_FORWARDPASS_H
#include "graphics/descriptors/DescriptorContainer.h"
#include "graphics/pipeline/Pipeline.h"
#include "systems/render/RenderPass.h"

namespace lisa::render {
  class ForwardPass : public systems::render::RenderPass {
  public:
    static constexpr auto TYPE_ID = "forward";

    explicit ForwardPass(const pugi::xml_node& node) : RenderPass(node) {}

    ~ForwardPass() override = default;
    void setup(
      systems::render::Rendergraph& graph, const pugi::xml_node& node
    ) override;
    void execute(const systems::render::RenderContext& ctx) override;

  private:
    const resources::Shader* shader_;
    uptr<graphics::Pipeline> pipeline_;
  };
}
#endif
