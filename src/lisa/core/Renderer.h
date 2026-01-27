//
// Created by kinami on 1/27/26.
//

#ifndef LISA_RENDERER_H
#define LISA_RENDERER_H


class Renderer
{
public:
    Renderer(const char* name, const unsigned int device) : name_(name), device_(device) {};
    ~Renderer() = default;

private:
    const char* name_;
    unsigned int device_;
};


#endif //LISA_RENDERER_H
