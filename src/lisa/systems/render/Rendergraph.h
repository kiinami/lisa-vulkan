//
// Created by kinami on 4/3/26.
//

#ifndef LISA_VULKAN_RENDERGRAPH_H
#define LISA_VULKAN_RENDERGRAPH_H
#include "RenderPass.h"
#include "RenderResource.h"
#include "utils/common.h"

namespace lisa::systems::render {
  template<typename T>
  concept RenderPassDerived = std::derived_from<T, RenderPass>;

  class Rendergraph {
  public:
    Rendergraph() = default;
    ~Rendergraph() = default;
    Rendergraph(Rendergraph&&) = default;
    Rendergraph& operator=(Rendergraph&&) = default;
    Rendergraph(const Rendergraph&) = delete;
    Rendergraph& operator=(const Rendergraph&) = delete;

    void add_resource(RenderResource res);

    template<RenderPassDerived T> void add_pass(const str& name) {
      auto resource = std::make_unique<T>(name);
      passes_.push_back(std::move(resource));
    }

    void compile();
    RenderResource* get_resource(const str& name);

    void render(
      const graphics::CommandBuffer& cmd_buffer,
      const scene::Scene& scene,
      vk::DeviceAddress global_bda,
      vk::DeviceAddress object_bda
    );

  private:
    struct ResourceState {
      vk::ImageLayout layout = vk::ImageLayout::eUndefined;
      vk::AccessFlags accessMask = vk::AccessFlagBits::eNone;
      vk::PipelineStageFlags stageMask = vk::PipelineStageFlagBits::eTopOfPipe;
    };

    umap<str, RenderResource> resources_;
    vector<std::unique_ptr<RenderPass>> passes_;
    vector<size> order_;
  };

}

#endif // LISA_VULKAN_RENDERGRAPH_H
