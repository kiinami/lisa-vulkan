//
// Created by kinami on 4/18/26.
//

#ifndef LISA_VULKAN_FORWARDPASS_H
#define LISA_VULKAN_FORWARDPASS_H
#include "graphics/descriptors/DescriptorContainer.h"
#include "render/resources/ColorRenderResource.h"
#include "render/resources/DepthRenderResource.h"
#include "render/resources/FinalRenderResource.h"
#include "systems/render/RenderPass.h"

namespace lisa::render::passes {

  class ForwardPass : public systems::render::RenderPass {
  public:
    static constexpr auto TYPE_ID = "forward";
    static inline const str SHADER = "shader";
    static inline const umap<
      LocalResourceID,
      const systems::render::RenderResourceDesc*>
      INPUTS = {};
    static inline const umap<
      LocalResourceID,
      const systems::render::RenderResourceDesc*>
      OUTPUTS = {
        {"color", &resources::final_attachment},
        {"depth", &resources::depth_attachment}
      };

    explicit ForwardPass(const pugi::xml_node& node) :
      RenderPass(node, INPUTS, OUTPUTS, SHADER) {}

    ~ForwardPass() override = default;

    void do_render(const systems::render::RenderContext& ctx) override;

  private:
  };

}

#endif // LISA_VULKAN_FORWARDPASS_H
