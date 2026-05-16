//
// Created by kinami on 4/3/26.
//

#ifndef LISA_VULKAN_RENDERGRAPH_H
#define LISA_VULKAN_RENDERGRAPH_H
#include "GraphResources.h"
#include "RenderPass.h"
#include "graphics/commands/CommandBuffer.h"
#include "graphics/images/Sampler.h"
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

    const ImageGraphResource& get_resource(const str& ref) const {
      return images_[resource_id_map_.at(ref)];
    }

    void render(
      const graphics::CommandBuffer& cmdb,
      vk::DeviceAddress global_bda,
      vk::DeviceAddress object_bda
    );

    const ImageGraphResource& output_resource() const {
      return images_[output_handle_];
    }

  private:
    struct ExecutionNode {
      RenderPass* pass;
      vk::Extent2D extent = {};
      uint32 layer_count = 1;
      vector<vk::ImageMemoryBarrier2> barriers;
      vector<vk::RenderingAttachmentInfo> color_attachments;
      optional<vk::RenderingAttachmentInfo> depth_attachment;
    };

    vector<ImageGraphResource> images_;
    umap<str, GraphResourceHandle> resource_id_map_;
    vector<uptr<RenderPass>> passes_;
    vector<ExecutionNode> nodes_;
    GraphResourceHandle output_handle_;
    umap<GraphResourceSamplerProfile, uptr<graphics::Sampler>> samplers_;

    void allocate_resources(const pugi::xml_node& doc_element);
    void allocate_passes(const pugi::xml_node& doc_element);
    void compile_graph(const pugi::xml_node& doc_element);
  };

}

#endif // LISA_VULKAN_RENDERGRAPH_H
