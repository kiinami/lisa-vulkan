//
// Created by kinami on 1/26/26.
//



#include "Application.h"
#include "../utils/chk.cpp"

Application::Application(const char* name) : name_(name)
{
}

Application::~Application()
{
}

int Application::run()
{
    std::cout << "Starting application " << name_;
    return 0;
}

