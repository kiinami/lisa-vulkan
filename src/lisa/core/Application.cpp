//
// Created by kinami on 1/26/26.
//

#define VOLK_IMPLEMENTATION
#include <volk/volk.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include "slang/slang.h"
#include <ktx.h>

#include "Application.h"
#include "../utils/chk.cpp"

Application::Application()
{
    init_libraries();
}

Application::~Application()
{
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_Quit();
}

int Application::run()
{
    std::cout << "Hello, word";
    return 0;
}

void Application::init_libraries()
{
    chk(SDL_Init(SDL_INIT_VIDEO));
    chk(SDL_Vulkan_LoadLibrary(nullptr));
    volkInitialize();
}
