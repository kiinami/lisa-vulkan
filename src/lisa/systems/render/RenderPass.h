//
// Created by kinami on 4/3/26.
//

#ifndef LISA_VULKAN_RENDERPASS_H
#define LISA_VULKAN_RENDERPASS_H

#include "ShaderData.h"
#include "graphics/commands/CommandBuffer.h"
#include "scene/Scene.h"

namespace lisa::systems::render {
  class Rendergraph;

  struct RenderContext {
    const scene::Scene& scene;
    const graphics::CommandBuffer& cmdb;
    vk::DeviceAddress global_bda = 0;
    vk::DeviceAddress object_bda = 0;
  };

  class RenderPass {
  public:
    explicit RenderPass(const pugi::xml_node& node) :
      id_(node.attribute("id").value()) {}

    virtual ~RenderPass() = default;

    const str& id() const { return id_; }

    virtual void setup(Rendergraph& graph, const pugi::xml_node& node) = 0;
    virtual void execute(const RenderContext& ctx) = 0;

  protected:
    str id_;
  };
}

#endif // LISA_VULKAN_RENDERPASS_H
