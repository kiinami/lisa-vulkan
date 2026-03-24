//
// Created by kinami on 1/26/26.
//

#include <CLI/CLI.hpp>
#include "lisa/graphics/graphics.h"
#include "lisa/graphics/device/Instance.h"
#include "lisa/utils/defer.h"
#include "lisa/utils/logging.h"

using namespace lisa;

namespace {
    CLI::App app{"lisa"};
    std::string log_level = "debug";
}

static int cli_args(int argc, char** argv)
{
    argv = app.ensure_utf8(argv);

    app.add_option(
        "-l,--log-level",
        log_level,
        "The logging level of the application"
        )
    ->check(CLI::IsMember({"trace","debug","info","warning","error","critical"}, CLI::ignore_case));

    CLI11_PARSE(app, argc, argv);
    return 0;
}

int main(int argc, char** argv)
{
    logging::init();

    auto result = cli_args(argc, argv);
    if (result != 0) return result;

    logging::set_level(log_level);

    {
        auto instance = Instance();

        graphics::init_device(instance);
        defer(graphics::destroy_device());
    }

    return 0;
}
