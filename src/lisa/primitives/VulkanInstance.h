//
// Created by kinami on 1/26/26.
//

#ifndef LISA_VULKANINSTANCE_H
#define LISA_VULKANINSTANCE_H

#include <vulkan/vulkan.h>

class VulkanInstance
{
public:
    explicit VulkanInstance(const char* applicationName);
    ~VulkanInstance();

private:
    VkInstance instance{VK_NULL_HANDLE};

    static void init_external_libraries();
    static void quit_external_libraries();
};


#endif //LISA_VULKANINSTANCE_H