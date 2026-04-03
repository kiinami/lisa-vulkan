//
// Created by kinami on 1/26/26.
//

#include "lisa/graphics/context.h"
#include "lisa/meshes/Mesh.h"
#include "lisa/utils/logging.h"
#include "window/context.h"

#include <CLI/CLI.hpp>

using namespace lisa;

namespace {
  CLI::App app{ "lisa" };
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
        { "trace", "debug", "info", "warning", "error", "critical" },
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

    {
      auto suzanne_path = std::filesystem::path("assets/models/suzanne.obj");
      auto suzanne = meshes::Mesh(suzanne_path);
    }

    graphics::context::destroy();
    window::context::destroy();
  }

  return 0;
}
