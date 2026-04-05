//
// Created by kinami on 4/3/26.
//

#include "Rendergraph.h"

#include "graphics/context.h"
#include "utils/logging.h"
#include "window/context.h"

#include <functional>

namespace lisa::systems::render {
  void Rendergraph::add_resource(RenderResource res) {
    resources_.insert_or_assign(res.name(), std::move(res));
  }

  // https://github.khronos.org/Vulkan-Site/tutorial/latest/Building_a_Simple_Engine/Engine_Architecture/05_rendering_pipeline.html#_rendergraph_dependency_analysis_and_execution_ordering
  void Rendergraph::compile() {
    vector<vector<size>> dependencies(passes_.size());
    vector<vector<size>> dependents(passes_.size());
    umap<str, size> writers;

    // Discover dependencies
    for (size i = 0; i < passes_.size(); i++) {
      const auto& pass = passes_[i];

      for (const auto& input : pass->inputs()) {
        if (auto it = writers.find(input.id); it != writers.end()) {
          dependencies[i].push_back(it->second);
          dependents[it->second].push_back(i);
        }
      }

      for (const auto& output : pass->outputs())
        writers[output.id] = i;
    }

    // Sort dependencies
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
  }

  RenderResource* Rendergraph::get_resource(const str& name) {
    return resources_.contains(name) ? &resources_.at(name) : nullptr;
  }

  void Rendergraph::render(
    const graphics::CommandBuffer& cmd_buffer,
    const scene::Scene& scene,
    vk::DeviceAddress global_bda,
    vk::DeviceAddress object_bda
  ) {
    umap<str, ResourceState> states(resources_.size());
    for (auto& [id, res] : resources_)
      states[id] = {
        .layout = res.image().initial_layout(),
        .accessMask = vk::AccessFlagBits::eNone,
        .stageMask = vk::PipelineStageFlagBits::eTopOfPipe
      };

    for (const auto pass_id : order_) {
      auto& pass = passes_[pass_id];
      vector<vk::ImageMemoryBarrier> barriers;

      auto transition_resource = [&](const RenderPass::ResourceUsage& res) {
        auto& [source_layout, source_access, source_stage] = states[res.id];
        auto& resource = resources_.at(res.id);

        if (
          source_layout !=
          res.layout ||
          (source_access & vk::AccessFlagBits::eMemoryWrite)
        ) {
          const vk::ImageMemoryBarrier barrier{
            .srcAccessMask = source_access,
            .dstAccessMask = res.access,
            .oldLayout = source_layout,
            .newLayout = res.layout,
            .image = resource.image(),
            .subresourceRange = {
              .aspectMask = res.aspect,
              .baseMipLevel = 0,
              .levelCount = 1,
              .baseArrayLayer = 0,
              .layerCount = 1
            }
          };

          cmd_buffer->pipelineBarrier(
            source_stage,
            res.stage,
            vk::DependencyFlagBits::eByRegion,
            nullptr,
            nullptr,
            barrier
          );

          source_layout = res.layout;
          source_access = res.access;
          source_stage = res.stage;
        }
      };

      for (const auto& input : pass->inputs())
        transition_resource(input);

      for (const auto& output : pass->outputs())
        transition_resource(output);

      RenderPass::RenderPassInput input{
        .cmd_buffer = cmd_buffer,
        .width = window::context::window_width(),
        .height = window::context::window_height(),
        .scene = scene,
        .global_bda = global_bda,
        .object_bda = object_bda
      };

      for (const auto& usage : pass->inputs()) {
        auto& resource = resources_.at(usage.id);
        input.image_views.insert(
          {usage.id,
           *resource.image().view(
             {.type = vk::ImageViewType::e2D,
              .format = resource.image().format(),
              .range = {
                .aspectMask = usage.aspect,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
              }}
           )}
        );
      }

      for (const auto& usage : pass->outputs()) {
        auto& resource = resources_.at(usage.id);
        input.image_views.insert(
          {usage.id,
           *resource.image().view(
             {.type = vk::ImageViewType::e2D,
              .format = resource.image().format(),
              .range = {
                .aspectMask = usage.aspect,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
              }}
           )}
        );
      }

      pass->render(input);
    }
  }
}
