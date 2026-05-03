//
// Created by kinami on 1/26/26.
//

#include "components/context.h"
#include "lisa/graphics/context.h"
#include "lisa/graphics/pipeline/Pipeline.h"
#include "lisa/utils/logging.h"
#include "resources/context.h"
#include "scene/gltf.h"
#include "scene/xml.h"
#include "systems/render/Renderer.h"
#include "update/context.h"
#include "window/context.h"
#include "window/events.h"

#include <CLI/CLI.hpp>

using namespace lisa;

namespace {
  CLI::App app{"lisa"};
  path scene_filepath;
  path rendergraph_filepath = "../assets/rendergraphs/deferred.xml";
  str log_level = "debug";
  int width = 800;
  int height = 800;
  int device = 0;
}

namespace {
  uptr<systems::render::Renderer> renderer_;

  bool should_close = false;
}

static optional<int> cli_args(int argc, char** argv) {
  argv = app.ensure_utf8(argv);

  app.add_option("scene", scene_filepath, "The XML scene file")
    ->check(CLI::ExistingFile)
    ->required();

  app
    .add_option("-g,--graph", rendergraph_filepath, "The XML rendergraph file")
    ->check(CLI::ExistingFile);

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

  app.add_option("--width", width, "The initial window width");
  app.add_option("--height", height, "The initial window height");
  app.add_option("-d,--device", device, "The GPU device to use");

  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError& e) { return app.exit(e); }
  return std::nullopt;
}

namespace {
  void on_window_resize(const window::events::WindowResize& event) {
    logging::debug("Window was resized, recreating renderer");
    graphics::context::device()->waitIdle();
    window::context::window().mark_dirty();
    renderer_.reset();
    graphics::context::recreate_swapchain();
    renderer_ =
      std::make_unique<systems::render::Renderer>(rendergraph_filepath);
  }

  REGISTER_EVENT(window::events::WindowResize, on_window_resize);

  void on_window_close(const window::events::WindowClose& event) {
    should_close = true;
  }

  REGISTER_EVENT(window::events::WindowClose, on_window_close);
}

int main(const int argc, char** argv) {
  if (const auto result = cli_args(argc, argv)) return *result;

  logging::init(log_level);

  {
    window::context::init(width, height);
    graphics::context::init();
    resources::context::init();
    components::context::init();
    update::context::init();

    {
      const auto ext = scene_filepath.extension().string();

      if (ext == ".xml")
        scene::xml::load(scene_filepath);
      else if (ext == ".gltf" || ext == ".glb")
        scene::gltf::load(scene_filepath);
      else
        logging::abort("Unsupported scene file format: '{}'", ext.c_str());

      renderer_ =
        std::make_unique<systems::render::Renderer>(rendergraph_filepath);

      while (!should_close) {
        window::context::dispatcher().update();
        window::context::poll_events();
        update::context::tick();

        renderer_->render();
      }

      graphics::context::device()->waitIdle();
      renderer_.reset();
    }

    update::context::destroy();
    components::context::destroy();
    resources::context::destroy();
    graphics::context::destroy();
    window::context::destroy();
  }

  return 0;
}
