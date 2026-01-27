//
// Created by kinami on 1/26/26.
//

#ifndef LISA_VULKAN_APPLICATION_H
#define LISA_VULKAN_APPLICATION_H
#include <memory>

// #include "Renderer.h"
// #include "VulkanRHI.h"
#include "Window.h"

class Application
{
public:
    Application();
    ~Application();

    void init(const std::string& name, unsigned int width, unsigned int height, unsigned int device);
    int run();

private:
    std::unique_ptr<Window> window_;
    // std::unique_ptr<VulkanRHI> vulkan_;
    // std::unique_ptr<Renderer> renderer_;
};


#endif //LISA_VULKAN_APPLICATION_H