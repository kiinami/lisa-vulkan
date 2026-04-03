//
// Created by kinami on 3/24/26.
//
#pragma once

// stdlib
#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// Vulkan
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_profiles.hpp>
#include <vulkan/vulkan_raii.hpp>

// SDL3
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <SDL3pp/SDL3pp.h>
#include <SDL3pp/SDL3pp_vulkan.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

// KTX
#include <ktx.h>
#include <ktxvulkan.h>

// Slang
#include <slang.h>
#include <slang/slang-com-helper.h>
#include <slang/slang-com-ptr.h>

// SPDLog
#include <spdlog/spdlog.h>

// tinyobjloader
#include <tiny_obj_loader.h>

// VMA
#include <vk_mem_alloc.h>
#include <vk_mem_alloc.hpp>
#include <vk_mem_alloc_raii.hpp>
