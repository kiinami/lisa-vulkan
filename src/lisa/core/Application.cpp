//
// Created by kinami on 1/26/26.
//

#include "Application.h"

#include "../utils/chk.cpp"

Application::Application()
{
}

Application::~Application()
{
}

void Application::init(const std::string& name, const unsigned int width, const unsigned int height, unsigned int device)
{
    window_ = std::make_unique<Window>(width, height, name);
}

int Application::run()
{
    std::cout << "Starting application...";
    return 0;
}

