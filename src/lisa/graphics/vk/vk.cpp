//
// Created by kinami on 3/24/26.
//

#include "vk.h"

#include "lisa/utils/chk.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <volk.h>

namespace lisa::graphics {
  void init_sdl() {
    utils::chk(SDL_Init(SDL_INIT_VIDEO));
    utils::chk(SDL_Vulkan_LoadLibrary(nullptr));
  }

  void init_volk() { volkInitialize(); }
}
