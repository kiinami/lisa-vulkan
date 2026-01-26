//
// Created by kinami on 1/26/26.
//

#include <iostream>
#include <vulkan/vulkan_core.h>

static void chk(const VkResult result)
{
    if (result != VK_SUCCESS)
    {
        std::cerr << "Vulkan call returned an error (" << result << ")\n";
        exit(result);
    }
}

static void chk(const bool result)
{
    if (!result)
    {
        std::cerr << "Call returned an error\n";
        exit(result);
    }
}
