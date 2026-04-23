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

    virtual ~RenderPass();

    const str& id() const { return id_; }

    virtual void setup(Rendergraph& graph, const pugi::xml_node& node) = 0;
    virtual void execute(const RenderContext& ctx) = 0;

    void set_input_index(const str& id, const uint32 index) {
      input_indices_[id] = index;
    }

  protected:
    str id_;
    umap<str, uint32> input_indices_;
  };
}

#endif // LISA_VULKAN_RENDERPASS_H
