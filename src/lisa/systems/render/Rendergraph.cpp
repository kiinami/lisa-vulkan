//
// Created by kinami on 4/3/26.
//

#include "Rendergraph.h"

#include "ImageRenderResource.h"
#include "RenderPassRegistry.h"
#include "RenderResourceRegistry.h"
#include "graphics/context.h"
#include "utils/logging.h"
#include "utils/xml.h"
#include "window/context.h"

#include <functional>

namespace lisa::systems::render {
  Rendergraph::Rendergraph(const path& filepath) {
    const auto doc = utils::xml::read(filepath, "rendergraph");
    const auto doc_element = doc.document_element();
    for (const auto& node : doc_element.children("pass"))
      passes_.push_back(
        RenderPassRegistry::create(node.attribute("type").value(), node)
      );
    output_id_ = doc_element.attribute("output").value();
    compile();
  }

  void Rendergraph::compile() {
    umap<str, size> writers;

    for (size i = 0; i < passes_.size(); i++) {
      const auto& descs = passes_[i]->output_descs();
      for (const auto& [local_id, global_id] : passes_[i]->output_entries()) {
        writers[global_id] = i;
        resources_.emplace(
          global_id,
          RenderResourceRegistry::create(
            descs.at(local_id)->type,
            global_id.c_str(),
            vec3(window::context::window_size(), 1.0f)
          )
        );
      }
    }

    vector dependents(passes_.size(), vector<size>{});

    for (size i = 0; i < passes_.size(); i++) {
      const auto& descs = passes_[i]->input_descs();
      for (const auto& [local_id, global_id] : passes_[i]->input_entries()) {
        auto it = writers.find(global_id);
        if (it == writers.end())
          logging::abort("No writer found for resource '{}'", global_id);

        if (resources_.at(global_id)->type() != descs.at(local_id)->type)
          logging::abort(
            "Resource type mismatch: pass '{}' expects '{}' for input '{}' but "
            "got '{}'",
            passes_[i]->id(),
            descs.at(local_id)->type,
            local_id,
            resources_.at(global_id)->type()
          );

        dependents[it->second].push_back(i);
      }
    }

    vector visited(passes_.size(), false);
    vector in_stack(passes_.size(), false);

    std::function<void(size)> visit = [&](const size node) {
      if (in_stack[node])
        logging::abort("Rendergraph has a circular dependency");
      if (visited[node]) return;
      in_stack[node] = true;
      for (const auto dep : dependents[node])
        visit(dep);
      in_stack[node] = false;
      visited[node] = true;
      order_.push_back(node);
    };

    for (size i = 0; i < passes_.size(); i++)
      if (!visited[i]) visit(i);

    if (resources_.at(output_id_)->type() != "final") {
      logging::abort(
        "Rendergraph output resource '{}' must be of type 'final', got '{}'",
        output_id_,
        resources_.at(output_id_)->type()
      );
    }
  }

  RenderResource* Rendergraph::get_resource(const str& id) const {
    return resources_.contains(id) ? resources_.at(id).get() : nullptr;
  }

  void Rendergraph::render(
    const graphics::CommandBuffer& cmdb,
    const scene::Scene& scene,
    const vk::DeviceAddress global_bda,
    const vk::DeviceAddress object_bda
  ) {
    for (const auto pass_id : order_) {
      const auto& pass = passes_[pass_id];

      umap<str, RenderResource*> local_resources;

      for (const auto& [local_id, global_id] : pass->input_entries()) {
        auto& resource = *resources_.at(global_id);
        if (auto* img = dynamic_cast<ImageRenderResource*>(&resource))
          img->transition(
            cmdb,
            static_cast<const ImageRenderResourceDesc&>(
              *pass->input_descs().at(local_id)
            )
          );
        local_resources[local_id] = &resource;
      }

      for (const auto& [local_id, global_id] : pass->output_entries()) {
        auto& resource = *resources_.at(global_id);
        if (auto* img = dynamic_cast<ImageRenderResource*>(&resource))
          img->transition(
            cmdb,
            static_cast<const ImageRenderResourceDesc&>(
              *pass->output_descs().at(local_id)
            )
          );
        local_resources[local_id] = &resource;
      }

      RenderContext ctx{scene, cmdb, local_resources, global_bda, object_bda};

      pass->render(ctx);
    }
  }
}
