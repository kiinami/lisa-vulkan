//
// Created by kinami on 1/27/26.
//

#include "Window.h"

#include <utility>

Window::Window(const unsigned int width, const unsigned int height, std::string title) : size_(width, height), title_(std::move(title))
{
    window_ = SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
    assert(window_);
}
