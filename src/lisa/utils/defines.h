//
// Created by kinami on 3/24/26.
//
#pragma once

#include <array>
#include <fstream>
#include <iostream>
#include <quill/Backend.h>
#include <quill/Frontend.h>
#include <quill/LogMacros.h>
#include <quill/Logger.h>
#include <quill/sinks/ConsoleSink.h>
#include <string>
#include <vector>
#include <volk.h>
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>
#define VP_VULKAN_PROFILES_IMPLEMENTATION
#include <vulkan/vulkan_profiles.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <tiny_obj_loader.h>

namespace lisa {
  typedef int8_t int8;
  typedef int32_t int32;
  typedef int64_t int64;
  typedef uint8_t uint8;
  typedef uint32_t uint32;
  typedef uint64_t uint64;

  typedef glm::vec2 Vector2f;
  typedef glm::vec3 Vector3f;
  typedef glm::vec4 Vector4f;

  typedef glm::quat Quaternion;

  typedef glm::mat3 Matrix3f;
  typedef glm::mat4 Matrix4f;
}
