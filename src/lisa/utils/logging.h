//
// Created by kinami on 3/24/26.
//

#ifndef LISA_LOGGING_H
#define LISA_LOGGING_H
#pragma once
#include <quill/Logger.h>
#include <string>


namespace lisa::logging
{
    void init();
    void set_level(const std::string& level);
    quill::Logger* get_logger();
}

#endif //LISA_LOGGING_H