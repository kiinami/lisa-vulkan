//
// Created by kinami on 1/26/26.
//

#include <volk/volk.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include "slang/slang.h"
#include <ktx.h>

#include "VulkanInstance.h"
#include "../utils/chk.cpp"

VulkanInstance::VulkanInstance(const char* applicationName)
{
    init_external_libraries();

    VkApplicationInfo appInfo{
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = applicationName,
        .apiVersion = VK_API_VERSION_1_3
    };

    uint32_t instanceExtensionsCount{0};
    char const* const* instanceExtensions{
        SDL_Vulkan_GetInstanceExtensions(&instanceExtensionsCount)
    };

    const VkInstanceCreateInfo instanceCI{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo,
        .enabledExtensionCount = instanceExtensionsCount,
        .ppEnabledExtensionNames = instanceExtensions,
    };
    chk(vkCreateInstance(&instanceCI, nullptr, &instance));
    volkLoadInstance(instance);
}

VulkanInstance::~VulkanInstance()
{
    vkDestroyInstance(instance, nullptr);
    quit_external_libraries();
}

void VulkanInstance::init_external_libraries()
{
    chk(SDL_Init(SDL_INIT_VIDEO));
    chk(SDL_Vulkan_LoadLibrary(nullptr));
    volkInitialize();
}

void VulkanInstance::quit_external_libraries()
{
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_Quit();
}
