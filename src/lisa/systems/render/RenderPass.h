//
// Created by kinami on 4/3/26.
//

#ifndef LISA_VULKAN_RENDERPASS_H
#define LISA_VULKAN_RENDERPASS_H

#include "RenderResource.h"
#include "ShaderData.h"
#include "graphics/buffer/Buffer.h"
#include "graphics/commands/CommandBuffer.h"
#include "graphics/pipeline/Pipeline.h"
#include "scene/Scene.h"
#include "systems/resources/ResourceManager.h"
#include "utils/common.h"

#include <pugixml.hpp>

namespace lisa::systems::render {

  struct RenderContext {
    const scene::Scene& scene;
    const graphics::CommandBuffer& cmdb;
    const umap<str, RenderResource*> resources;
    vk::DeviceAddress global_bda = 0;
    vk::DeviceAddress object_bda = 0;
  };

  class RenderPass {
  public:
    using GlobalResourceID = str;
    using LocalResourceID = str;

    explicit RenderPass(
      const pugi::xml_node& node,
      const umap<LocalResourceID, const RenderResourceDesc*>& valid_inputs,
      const umap<LocalResourceID, const RenderResourceDesc*>& valid_outputs,
      const str& shader_id
    );
    virtual ~RenderPass() = default;

    const str& id() { return id_; }

    umap<LocalResourceID, GlobalResourceID> input_entries() const {
      umap<LocalResourceID, GlobalResourceID> result;
      for (const auto& [local_id, id] : inputs_map_)
        result[local_id] = id_ + "." + id;
      return result;
    }

    umap<LocalResourceID, GlobalResourceID> output_entries() const {
      umap<LocalResourceID, GlobalResourceID> result;
      for (const auto& [local_id, id] : outputs_map_)
        result[local_id] = id_ + "." + id;
      return result;
    }

    const umap<LocalResourceID, str>& input_types() const {
      return input_types_;
    }

    const umap<LocalResourceID, str>& output_types() const {
      return output_types_;
    }

    const umap<LocalResourceID, const RenderResourceDesc*>&
      input_descs() const {
      return input_descs_;
    }

    const umap<LocalResourceID, const RenderResourceDesc*>&
      output_descs() const {
      return output_descs_;
    }

    void render(const RenderContext& ctx);

    virtual void do_render(const RenderContext& ctx) = 0;

  protected:
    str id_;
    umap<LocalResourceID, str> input_types_;
    umap<LocalResourceID, str> output_types_;
    umap<LocalResourceID, GlobalResourceID> inputs_map_;
    umap<LocalResourceID, GlobalResourceID> outputs_map_;
    umap<LocalResourceID, const RenderResourceDesc*> input_descs_;
    umap<LocalResourceID, const RenderResourceDesc*> output_descs_;
    resources::ResourceHandle<lisa::resources::Shader> shader_;
    uptr<graphics::Pipeline> pipeline_;
  };

}

#endif // LISA_VULKAN_RENDERPASS_H
