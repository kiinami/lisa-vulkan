//
// Created by kinami on 3/24/26.
//
#pragma once

// stdlib
#include <algorithm>
#include <any>
#include <array>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <random>
#include <ranges>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

// Vulkan
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_profiles.hpp>
#include <vulkan/vulkan_raii.hpp>

// SDL3
#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_vulkan.h>

// SDL3pp
#include <SDL3pp/SDL3pp.h>
#include <SDL3pp/SDL3pp_init.h>
#include <SDL3pp/SDL3pp_vulkan.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

// KTX
#include <ktx.h>
#include <ktxvulkan.h>

// Slang
#include <slang/slang-com-helper.h>
#include <slang/slang-com-ptr.h>
#include <slang/slang.h>

// SPDLog
#include <spdlog/sinks/stdout_color_sinks-inl.h>
#include <spdlog/spdlog.h>

// tinyobjloader
#include <tiny_obj_loader.h>

// VMA
#include <vk_mem_alloc.h>
#include <vk_mem_alloc.hpp>
#include <vk_mem_alloc_raii.hpp>

// entt
#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
#include <entt/entt.hpp>
#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>

// better-enum
#include <enum.h>

// stb
#include <stb_image.h>

// tinyexr
#include <tinyexr.h>
