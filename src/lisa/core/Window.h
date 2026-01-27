//
// Created by kinami on 1/27/26.
//

#ifndef LISA_WINDOW_H
#define LISA_WINDOW_H
#include <string>
#include <glm/vec2.hpp>
#include <SDL3/SDL_video.h>


class Window
{
public:
    Window(unsigned int width, unsigned int height, std::string title);
    ~Window() = default;

    [[nodiscard]] SDL_Window* window() const {return window_;}
private:
    glm::ivec2 size_{};
    std::string title_;

    SDL_Window* window_;
};


#endif //LISA_WINDOW_H