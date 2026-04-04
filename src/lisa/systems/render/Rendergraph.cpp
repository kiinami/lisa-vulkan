//
// Created by kinami on 4/3/26.
//

#include "Rendergraph.h"

#include "graphics/context.h"
#include "utils/logging.h"

#include <functional>

namespace lisa::systems::render {
  void Rendergraph::add_resource(RenderResource res) {
    resources_.insert_or_assign(res.name(), std::move(res));
  }

  void Rendergraph::add_pass(const RenderPass& pass) {
    passes_.push_back(pass);
  }

  // https://github.khronos.org/Vulkan-Site/tutorial/latest/Building_a_Simple_Engine/Engine_Architecture/05_rendering_pipeline.html#_rendergraph_dependency_analysis_and_execution_ordering
  void Rendergraph::compile() {
    vector<vector<size>> dependencies(passes_.size());
    vector<vector<size>> dependents(passes_.size());
    umap<str, size> writers;

    // Discover dependencies
    for (size i = 0; i < passes_.size(); i++) {
      const auto& pass = passes_[i];

      for (const auto& input : pass.inputs()) {
        if (auto it = writers.find(input); it != writers.end()) {
          dependencies[i].push_back(it->second);
          dependents[it->second].push_back(i);
        }
      }

      for (const auto& output : pass.outputs())
        writers[output] = i;
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

    // Semaphore creation
    for (size i = 0; i < passes_.size(); i++)
      for (auto dep : dependencies[i]) {
        semaphores_.emplace_back(
          graphics::context::device()->createSemaphore({})
        );
        semaphore_signal_pairs_.emplace_back(dep, i);
      }
  }

  RenderResource* Rendergraph::get_resource(const str& name) {
    return resources_.contains(name) ? &resources_.at(name) : nullptr;
  }

  void Rendergraph::render(const graphics::CommandBuffer& cmd_buffer) {
    umap<str, ResourceState> states(resources_.size());
    for (auto& [id, res] : resources_)
      states[id] = {
        .layout = res.image().initial_layout(),
        .accessMask = vk::AccessFlagBits::eNone,
        .stageMask = vk::PipelineStageFlagBits::eTopOfPipe
      };

    for (const auto pass_id : order_) {
      const auto& pass = passes_[pass_id];
      vector<vk::ImageMemoryBarrier> barriers;

      auto transition_resource = [&](
                                   const str& resource_id,
                                   const vk::ImageLayout target_layout,
                                   const vk::AccessFlags target_access,
                                   const vk::PipelineStageFlags target_stage
                                 ) {
        auto& [source_layout, source_access, source_stage] =
          states[resource_id];
        auto& resource = resources_.at(resource_id);

        if (
          source_layout !=
          target_layout ||
          (source_access & vk::AccessFlagBits::eMemoryWrite)
        ) {
          const vk::ImageMemoryBarrier barrier{
            .srcAccessMask = source_access,
            .dstAccessMask = target_access,
            .oldLayout = source_layout,
            .newLayout = target_layout,
            .image = resource.image(),
            .subresourceRange = {
              .aspectMask = vk::ImageAspectFlagBits::eColor,
              .baseMipLevel = 0,
              .levelCount = 1,
              .baseArrayLayer = 0,
              .layerCount = 1
            }
          };

          cmd_buffer->pipelineBarrier(
            source_stage,
            target_stage,
            vk::DependencyFlagBits::eByRegion,
            nullptr,
            nullptr,
            barrier
          );

          source_layout = target_layout;
          source_access = target_access;
          source_stage = target_stage;
        }
      };

      for (const auto& input : pass.inputs())
        transition_resource(
          input,
          vk::ImageLayout::eShaderReadOnlyOptimal,
          vk::AccessFlagBits::eShaderRead,
          vk::PipelineStageFlagBits::eFragmentShader
        );

      for (const auto& output : pass.outputs())
        transition_resource(
          output,
          vk::ImageLayout::eColorAttachmentOptimal,
          vk::AccessFlagBits::eColorAttachmentWrite,
          vk::PipelineStageFlagBits::eColorAttachmentOutput
        );

      pass.render_function(cmd_buffer);
    }
  }
}
