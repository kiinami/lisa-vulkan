//
// Created by kinami on 4/3/26.
//

#ifndef LISA_VULKAN_RENDERGRAPH_H
#define LISA_VULKAN_RENDERGRAPH_H
#include "ImageRenderResource.h"
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

    explicit Rendergraph(const path& filepath);

    RenderResource* get_resource(const str& id) const;

    const ImageRenderResource* output_resource() const {
      return static_cast<const ImageRenderResource*>(
        resources_.at(output_id_).get()
      );
    }

    void render(
      const graphics::CommandBuffer& cmdb,
      const scene::Scene& scene,
      vk::DeviceAddress global_bda,
      vk::DeviceAddress object_bda
    );

  private:
    umap<str, uptr<RenderResource>> resources_;
    vector<uptr<RenderPass>> passes_;
    vector<size> order_;
    str output_id_;

    void compile();
  };

}

#endif // LISA_VULKAN_RENDERGRAPH_H
