//
// Created by kinami on 4/3/26.
//

#ifndef LISA_VULKAN_RENDERGRAPH_H
#define LISA_VULKAN_RENDERGRAPH_H
#include "RenderPass.h"
#include "RenderResource.h"
#include "utils/common.h"

namespace lisa::systems::render {

  class Rendergraph {
  public:
    Rendergraph() = default;
    ~Rendergraph() = default;
    Rendergraph(Rendergraph&&) = default;
    Rendergraph& operator=(Rendergraph&&) = default;
    Rendergraph(const Rendergraph&) = delete;
    Rendergraph& operator=(const Rendergraph&) = delete;

    void add_resource(RenderResource res);
    void add_pass(const RenderPass& pass);
    void compile();
    RenderResource* get_resource(const str& name);

    void render(const graphics::CommandBuffer& cmd_buffer);

  private:
    struct ResourceState {
      vk::ImageLayout layout = vk::ImageLayout::eUndefined;
      vk::AccessFlags accessMask = vk::AccessFlagBits::eNone;
      vk::PipelineStageFlags stageMask = vk::PipelineStageFlagBits::eTopOfPipe;
    };

    umap<str, RenderResource> resources_;
    vector<RenderPass> passes_;
    vector<size> order_;
  };

}

#endif // LISA_VULKAN_RENDERGRAPH_H
