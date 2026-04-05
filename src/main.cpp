//
// Created by kinami on 1/26/26.
//

#include "lisa/graphics/context.h"
#include "lisa/graphics/descriptors/DescriptorContainer.h"
#include "lisa/graphics/pipeline/Pipeline.h"
#include "lisa/utils/logging.h"
#include "passes/DefaultPass.h"
#include "resources/context.h"
#include "scene/Scene.h"
#include "systems/render/Renderer.h"
#include "systems/render/Rendergraph.h"
#include "systems/resources/ResourceManager.h"
#include "window/context.h"

#include <CLI/CLI.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace lisa;

namespace {
  CLI::App app{"lisa"};
  str log_level = "debug";
  int device = 0;
}

static int cli_args(int argc, char** argv) {
  argv = app.ensure_utf8(argv);

  app
    .add_option(
      "-l,--log-level", log_level, "The logging level of the application"
    )
    ->check(
      CLI::IsMember(
        {"trace", "debug", "info", "warning", "error", "critical"},
        CLI::ignore_case
      )
    );

  app.add_option("-d,--device", device, "The GPU device to use");

  CLI11_PARSE(app, argc, argv);
  return 0;
}

int main(int argc, char** argv) {
  auto result = cli_args(argc, argv);
  if (result != 0) return result;

  logging::init(log_level);

  {
    window::context::init(1280, 720);
    graphics::context::init();
    resources::context::init();

    {
      vector<scene::Entity> entities;
      for (uint32 i = 0; i < 3; i++) {
        float x_offset = (i - 1.0f) * 2.0f;
        entities.push_back(
          {.mesh_id = "suzanne",
           .texture_id = "suzanne" + std::to_string(i),
           .model = glm::translate(mat4(1.0f), vec3(x_offset, 0.0f, 0.0f))}
        );
      }

      auto scene = scene::Scene(
        entities,
        {.width = window::context::window_width(),
         .height = window::context::window_height(),
         .view = glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, -5.0f)),
         .projection = glm::perspective(
           glm::radians(45.0f),
           static_cast<float>(window::context::window_width()) /
             static_cast<float>(window::context::window_height()),
           0.1f,
           100.0f
         )}
      );

      auto rendergraph = systems::render::Rendergraph();
      rendergraph.add_resource(
        {"FinalTarget",
         vk::Format::eR8G8B8A8Unorm,
         {window::context::window_width(), window::context::window_height(), 1},
         vk::ImageUsageFlagBits::eColorAttachment |
           vk::ImageUsageFlagBits::eTransferSrc}
      );
      rendergraph.add_resource(
        {"DepthTarget",
         vk::Format::eD32Sfloat,
         {window::context::window_width(), window::context::window_height(), 1},
         vk::ImageUsageFlagBits::eDepthStencilAttachment}
      );

      rendergraph.add_pass<passes::DefaultPass>("GeometryPass");
      rendergraph.compile();

      auto renderer =
        std::make_unique<systems::render::Renderer>(std::move(rendergraph));

      while (!window::context::should_close()) {
        window::context::poll_events();
        renderer->render(scene);
      }

      graphics::context::device()->waitIdle();
    }

    resources::context::destroy();
    graphics::context::destroy();
    window::context::destroy();
  }

  return 0;
}
