//
// Created by kinami on 3/24/26.
//

#ifndef LISA_GRAPHICS_H
#define LISA_GRAPHICS_H
#include <vulkan/vulkan_core.h>

#include "device/Instance.h"


namespace lisa::graphics
{
    void init_device(Instance& instance);
    void destroy_device();

    VkInstance instance();
    VkDevice device();
    VkPhysicalDevice physical_device();
}


#endif //LISA_GRAPHICS_H