//
// Created by kinami on 1/26/26.
//

#ifndef LISA_VULKAN_APPLICATION_H
#define LISA_VULKAN_APPLICATION_H
#include "../primitives/VulkanInstance.h"


class Application
{
public:
    explicit Application(const char* name);
    ~Application();

    int run();

private:
    const char* name_;
    VulkanInstance instance_{name_};
};


#endif //LISA_VULKAN_APPLICATION_H