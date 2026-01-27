//
// Created by kinami on 1/26/26.
//

#ifndef LISA_VULKAN_APPLICATION_H
#define LISA_VULKAN_APPLICATION_H
#include <memory>

#include "Renderer.h"
#include "VulkanRHI.h"
#include "Window.h"

class Application
{
public:
    Application();
    ~Application();

    void init(const char* name, unsigned int device);
    int run();

private:
    const char* name_ = "lisa";
    unsigned int device_ = 0;

    std::unique_ptr<Window> window_;
    std::unique_ptr<VulkanRHI> vulkan_;
    std::unique_ptr<Renderer> renderer_;
};


#endif //LISA_VULKAN_APPLICATION_H