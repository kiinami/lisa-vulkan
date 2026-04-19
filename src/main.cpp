//
// Created by kinami on 1/26/26.
//

#include "components/context.h"
#include "lisa/graphics/context.h"
#include "lisa/graphics/descriptors/DescriptorContainer.h"
#include "lisa/graphics/pipeline/Pipeline.h"
#include "lisa/utils/logging.h"
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
  path scene_filepath;
  str log_level = "debug";
  int device = 0;
}

static int cli_args(int argc, char** argv) {
  argv = app.ensure_utf8(argv);

  app.add_option("scene", scene_filepath, "The XML scene file")
    ->check(CLI::ExistingFile)
    ->required();

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

int main(const int argc, char** argv) {
  const auto result = cli_args(argc, argv);
  if (result != 0) return result;

  logging::init(log_level);

  {
    window::context::init(1280, 720);
    graphics::context::init();
    resources::context::init();
    components::context::init();

    {
      auto scene = scene::Scene(scene_filepath);

      auto renderer = std::make_unique<systems::render::Renderer>(
        "assets/rendergraphs/forward.xml"
      );

      while (!window::context::should_close()) {
        window::context::poll_events();
        renderer->render(scene);
      }

      graphics::context::device()->waitIdle();
    }

    components::context::destroy();
    resources::context::destroy();
    graphics::context::destroy();
    window::context::destroy();
  }

  return 0;
}
