//
// Created by kinami on 3/24/26.
//

#include "logging.h"
#include <quill/Backend.h>
#include <quill/Frontend.h>
#include <quill/LogMacros.h>
#include <quill/Logger.h>
#include <quill/sinks/ConsoleSink.h>

namespace lisa::logging
{
    using namespace quill;

    namespace
    {
        std::shared_ptr<Sink> sink;
        Logger* logger;
    }

    void init()
    {
        Backend::start();
        sink = Frontend::create_or_get_sink<ConsoleSink>("lisa");
        logger = Frontend::create_or_get_logger("root", std::move(sink));
    }

    void set_level(const std::string& level)
    {
        if (level == "trace")
            logger->set_log_level(LogLevel::TraceL1);
        else if (level == "debug")
            logger->set_log_level(LogLevel::Debug);
        else if (level == "info")
            logger->set_log_level(LogLevel::Info);
        else if (level == "warning")
            logger->set_log_level(LogLevel::Warning);
        else if (level == "error")
            logger->set_log_level(LogLevel::Error);
        else if (level == "critical")
            logger->set_log_level(LogLevel::Critical);
    }

    Logger* get_logger() { return logger; }
}
