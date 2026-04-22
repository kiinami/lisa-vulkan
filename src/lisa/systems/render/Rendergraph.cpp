//
// Created by kinami on 4/3/26.
//

#include "Rendergraph.h"

#include "GraphResources.h"
#include "RenderPassRegistry.h"
#include "graphics/context.h"
#include "utils/logging.h"
#include "utils/xml.h"
#include "window/context.h"

#include <functional>

namespace lisa::systems::render {
  Rendergraph::Rendergraph(const path& filepath) {
    const auto doc = utils::xml::read(filepath, "rendergraph");
    const auto doc_element = doc.document_element();

    allocate_resources(doc_element);
    allocate_passes(doc_element);
    compile_graph(doc_element);

    output_handle_ =
      resource_id_map_.at(doc_element.attribute("output").value());
  }

  void Rendergraph::allocate_resources(const pugi::xml_node& doc_element) {
    for (const auto& node : doc_element.children("resource")) {
      str id = node.attribute("id").value();
      const auto metadata =
        ImageGraphResourceMetadata::from_type(node.attribute("type").value());
      auto size =
        window::context::from_scale(node.attribute("scale").as_float(1.0f));

      resource_id_map_[id] = static_cast<GraphResourceHandle>(images_.size());
      auto image = ImageGraphResource(metadata, vec3(size, 1.0f));
      images_.push_back(std::move(image));
    }
  }

  void Rendergraph::allocate_passes(const pugi::xml_node& doc_element) {
    for (const auto& node : doc_element.children("pass"))
      passes_.push_back(
        RenderPassRegistry::create(node.attribute("type").value(), node)
      );
  }

  void Rendergraph::compile_graph(const pugi::xml_node& doc_element) {
    umap<str, size> writers;
    vector<vector<size>> dependents(passes_.size());

    uint32_t pass_idx = 0;
    for (const auto& pass_node : doc_element.children("pass")) {
      for (const auto& output : pass_node.children("output")) {
        str ref = output.attribute("ref").value();
        writers[ref] = pass_idx;
      }
      for (const auto& input : pass_node.children("input")) {
        str ref = input.attribute("ref").value();
        dependents[writers[ref]].push_back(pass_idx);
      }
      pass_idx++;
    }

    vector visited(passes_.size(), false);
    vector in_stack(passes_.size(), false);
    vector<size> order;

    std::function<void(size)> visit = [&](const size node) {
      if (in_stack[node])
        logging::abort("Rendergraph has a circular dependency");
      if (visited[node]) return;

      in_stack[node] = true;
      for (const auto dep : dependents[node])
        visit(dep);
      in_stack[node] = false;
      visited[node] = true;

      order.push_back(node);
    };

    for (size i = 0; i < passes_.size(); i++)
      if (!visited[i]) visit(i);

    std::ranges::reverse(order);

    vector<GraphResourceState> resource_states(images_.size());
    vector has_been_written(images_.size(), false);

    for (size pass_idx : order) {
      const auto& pass_node =
        *std::next(doc_element.children("pass").begin(), pass_idx);

      ExecutionNode exec_node;
      exec_node.pass = passes_[pass_idx].get();

      exec_node.pass->setup(*this, pass_node);

      for (const auto& input : pass_node.children("input")) {
        str ref = input.attribute("ref").value();
        auto handle = resource_id_map_.at(ref);
        auto& img = images_[handle];

        auto usage = utils::xml::parse_enum<GraphResourceUsage>(
          input.attribute("usage").value()
        );

        vk::ImageMemoryBarrier2 barrier =
          img.transition(resource_states[handle], usage);
        exec_node.barriers.push_back(barrier);

        if (usage == DepthStencilAttachmentRead) {
          vk::RenderingAttachmentInfo attachment =
            img.attachment_info(true, true);
          exec_node.depth_attachment = attachment;
        }
      }

      for (const auto& output : pass_node.children("output")) {
        str ref = output.attribute("ref").value();
        auto handle = resource_id_map_.at(ref);
        auto& img = images_[handle];
        auto usage = utils::xml::parse_enum<GraphResourceUsage>(
          output.attribute("usage").value()
        );

        vk::ImageMemoryBarrier2 barrier =
          img.transition(resource_states[handle], usage);
        exec_node.barriers.push_back(barrier);

        vk::RenderingAttachmentInfo attachment =
          img.attachment_info(has_been_written[handle]);

        if (img.format().is_depth())
          exec_node.depth_attachment = attachment;
        else
          exec_node.color_attachments.push_back(attachment);

        has_been_written[handle] = true;
      }
      nodes_.push_back(std::move(exec_node));
    }
  }

  void Rendergraph::render(
    const graphics::CommandBuffer& cmdb,
    const scene::Scene& scene,
    const vk::DeviceAddress global_bda,
    const vk::DeviceAddress object_bda
  ) {
    for (const auto& [pass, barriers, color_attachments, depth_attachment] :
         nodes_) {
      if (!barriers.empty()) {
        vk::DependencyInfo dep_info{
          .imageMemoryBarrierCount = static_cast<uint32_t>(barriers.size()),
          .pImageMemoryBarriers = barriers.data()
        };
        cmdb->pipelineBarrier2(dep_info);
      }

      const bool has_attachments =
        !color_attachments.empty() || depth_attachment.has_value();

      if (has_attachments) {
        vk::RenderingInfo render_info{
          .renderArea = {.extent = graphics::context::swapchain().extent()},
          .layerCount = 1,
          .colorAttachmentCount =
            static_cast<uint32_t>(color_attachments.size()),
          .pColorAttachments = color_attachments.data(),
          .pDepthAttachment =
            depth_attachment ? &depth_attachment.value() : nullptr
        };
        cmdb->beginRendering(render_info);

        const auto extent = graphics::context::swapchain().extent();
        cmdb->setViewport(
          0,
          {{.width = static_cast<float>(extent.width),
            .height = static_cast<float>(extent.height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f}}
        );
        cmdb->setScissor(0, {{.extent = extent}});
      }

      RenderContext ctx{scene, cmdb, global_bda, object_bda};
      pass->execute(ctx);

      if (has_attachments) cmdb->endRendering();
    }
  }
}
