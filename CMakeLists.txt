# Header ######################################################################
cmake_minimum_required(VERSION 3.25)

project(lisa VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# Options #####################################################################

set(VULKAN_SDK "" CACHE PATH "Path to the Vulkan SDK root (e.g. /home/user/vulkan/1.x.x.x/x86_64)")

# Compile definitions #########################################################

add_compile_definitions(
        NOMINMAX
        _USE_MATH_DEFINES
        VULKAN_HPP_NO_CONSTRUCTORS
        VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
        VP_USE_OBJECT
        GLM_FORCE_RADIANS
        GLM_FORCE_DEPTH_ZERO_TO_ONE
        GLM_ENABLE_EXPERIMENTAL
        MAGIC_ENUM_RANGE_MIN=0
        MAGIC_ENUM_RANGE_MAX=2000
        $<$<CONFIG:Debug>:DEBUG>
)

# Git dependencies ############################################################

set(SDL3PP_FORCE_BUNDLED ON)
set(SDL3PP_ENABLE_IMAGE OFF)
set(SDL3PP_ENABLE_MIXER OFF)
set(SDL3PP_ENABLE_NET OFF)
set(SDL3PP_ENABLE_TTF OFF)
FetchContent_Declare(
        sdl3pp
        URL https://github.com/talesm/SDL3pp/releases/download/0.10.1/SDL3pp-0.10.1.tar.gz
        URL_HASH SHA256=edc5dc16a97cec42e98c5d50ef1b5311073daa216e298c164ba00542787c7233
)

FetchContent_Declare(
        vmahpp
        URL https://github.com/YaaZ/VulkanMemoryAllocator-Hpp/releases/download/v3.3.0%2B3/VulkanMemoryAllocator-Hpp-3.3.0.tar.gz
        URL_HASH SHA256=8c2a0573babe1f86f3241d12b4c9df40bb944649b7f4239c1c611797f8af6cd5
)

FetchContent_Declare(
        better-enums
        URL https://github.com/aantron/better-enums/archive/refs/tags/0.11.3.tar.gz
        URL_HASH SHA256=1b1597f0aa5452b971a94ab13d8de3b59cce17d9c43c8081aa62f42b3376df96
)

FetchContent_MakeAvailable(sdl3pp vmahpp better-enums)

if (WIN32)
    set(SDL3PP_RENDER_H "${sdl3pp_SOURCE_DIR}/include/SDL3pp/SDL3pp_render.h")
    if (EXISTS "${SDL3PP_RENDER_H}")
        file(READ "${SDL3PP_RENDER_H}" _sdl3pp_content)
        string(REGEX REPLACE "const[ \t\r\n]+(SDL::)?GPURenderStateCreateInfo" "\\1GPURenderStateCreateInfo" _sdl3pp_content "${_sdl3pp_content}")
        file(WRITE "${SDL3PP_RENDER_H}" "${_sdl3pp_content}")
    endif ()
endif ()

add_library(better-enums INTERFACE)
target_include_directories(better-enums INTERFACE ${better-enums_SOURCE_DIR})

# Vulkan SDK ##################################################################

if (NOT VULKAN_SDK AND DEFINED ENV{VULKAN_SDK})
    set(VULKAN_SDK "$ENV{VULKAN_SDK}" CACHE PATH "Path to the Vulkan SDK root" FORCE)
endif ()

if (NOT VULKAN_SDK)
    message(FATAL_ERROR "VULKAN_SDK is not set! Please set the VULKAN_SDK CMake variable or environment variable to ensure the SDK tools are used.")
endif ()

message(STATUS "Using Vulkan SDK at: ${VULKAN_SDK}")
set(Vulkan_INCLUDE_DIR "${VULKAN_SDK}/include")

if (WIN32)
    find_library(Vulkan_LIBRARY NAMES vulkan-1 PATHS "${VULKAN_SDK}/lib" REQUIRED NO_DEFAULT_PATH)
else ()
    find_library(Vulkan_LIBRARY NAMES vulkan PATHS "${VULKAN_SDK}/lib" REQUIRED NO_DEFAULT_PATH)
endif ()

add_library(Vulkan::Vulkan UNKNOWN IMPORTED)
set_target_properties(Vulkan::Vulkan PROPERTIES
        IMPORTED_LOCATION "${Vulkan_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${Vulkan_INCLUDE_DIR}"
)

find_library(Slang_LIBRARY NAMES slang PATHS "${VULKAN_SDK}/lib" REQUIRED NO_DEFAULT_PATH)
add_library(slang::slang UNKNOWN IMPORTED)
set_target_properties(slang::slang PROPERTIES
        IMPORTED_LOCATION "${Slang_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${Vulkan_INCLUDE_DIR}"
)

# Conan dependencies ##########################################################

find_package(Boost REQUIRED COMPONENTS headers)
find_package(CLI11 REQUIRED)
find_package(EnTT REQUIRED)
find_package(fastgltf REQUIRED)
find_package(glm REQUIRED)
find_package(Ktx REQUIRED)
find_package(miniz REQUIRED)
find_package(pugixml REQUIRED)
find_package(reflectcpp REQUIRED)
find_package(spdlog REQUIRED)
find_package(Stb REQUIRED)
find_package(tinyexr REQUIRED)
find_package(tinyobjloader REQUIRED)
find_package(VulkanMemoryAllocator REQUIRED)

# Sources #####################################################################

set(LISA_SOURCES
        src/lisa/build.h
        src/lisa/constants.h
        src/lisa/implementations.cpp

        src/lisa/graphics/vk/VkObject.h
        src/lisa/graphics/vk/VkObject.cpp
        src/lisa/graphics/vk/NamedVkObject.h
        src/lisa/graphics/vk/NamedVkObject.cpp
        src/lisa/graphics/vk/VkEnum.h
        src/lisa/graphics/vk/VkEnum.cpp

        src/lisa/graphics/buffer/Buffer.h
        src/lisa/graphics/buffer/Buffer.cpp

        src/lisa/graphics/acceleration/AccelerationStructure.h
        src/lisa/graphics/acceleration/AccelerationStructure.cpp

        src/lisa/graphics/descriptors/DescriptorAllocator.h
        src/lisa/graphics/descriptors/DescriptorAllocator.cpp
        src/lisa/graphics/descriptors/DescriptorContainer.h
        src/lisa/graphics/descriptors/DescriptorContainer.cpp

        src/lisa/graphics/device/Instance.h
        src/lisa/graphics/device/Instance.cpp
        src/lisa/graphics/device/PhysicalDevice.h
        src/lisa/graphics/device/PhysicalDevice.cpp
        src/lisa/graphics/device/LogicalDevice.h
        src/lisa/graphics/device/LogicalDevice.cpp

        src/lisa/graphics/commands/CommandBuffer.h
        src/lisa/graphics/commands/CommandBuffer.cpp

        src/lisa/graphics/images/Image.h
        src/lisa/graphics/images/Image.cpp
        src/lisa/graphics/images/ImageFormat.h
        src/lisa/graphics/images/ImageFormat.cpp
        src/lisa/graphics/images/Sampler.h
        src/lisa/graphics/images/Sampler.cpp

        src/lisa/graphics/memory/MemoryAllocator.h
        src/lisa/graphics/memory/MemoryAllocator.cpp

        src/lisa/graphics/pipeline/ShaderModule.h
        src/lisa/graphics/pipeline/ShaderModule.cpp
        src/lisa/graphics/pipeline/PipelineLayout.h
        src/lisa/graphics/pipeline/PipelineLayout.cpp
        src/lisa/graphics/pipeline/Pipeline.h
        src/lisa/graphics/pipeline/Pipeline.cpp

        src/lisa/graphics/swapchain/Surface.h
        src/lisa/graphics/swapchain/Surface.cpp
        src/lisa/graphics/swapchain/Swapchain.h
        src/lisa/graphics/swapchain/Swapchain.cpp

        src/lisa/graphics/sync/Fence.h
        src/lisa/graphics/sync/Fence.cpp
        src/lisa/graphics/sync/Semaphore.h
        src/lisa/graphics/sync/Semaphore.cpp

        src/lisa/graphics/context.h
        src/lisa/graphics/context.cpp

        src/lisa/resources/context.h
        src/lisa/resources/context.cpp
        src/lisa/resources/Mesh.h
        src/lisa/resources/Mesh.cpp
        src/lisa/resources/Shader.h
        src/lisa/resources/Shader.cpp
        src/lisa/resources/Texture.h
        src/lisa/resources/Texture.cpp
        src/lisa/resources/Vertex.h

        src/lisa/render/ShaderPass.h
        src/lisa/render/ShaderPass.cpp
        src/lisa/render/ForwardPass.h
        src/lisa/render/ForwardPass.cpp
        src/lisa/render/GeometryPass.h
        src/lisa/render/GeometryPass.cpp
        src/lisa/render/CompositionPass.h
        src/lisa/render/CompositionPass.cpp
        src/lisa/render/CompositionRtxPass.h
        src/lisa/render/CompositionRtxPass.cpp
        src/lisa/render/DepthPass.h
        src/lisa/render/DepthPass.cpp
        src/lisa/render/SSAOPass.h
        src/lisa/render/SSAOPass.cpp
        src/lisa/render/ConvolutionalPass.h
        src/lisa/render/ConvolutionalPass.cpp
        src/lisa/render/ShadowPass.h
        src/lisa/render/ShadowPass.cpp
        src/lisa/render/ShadowRtxPass.h
        src/lisa/render/ShadowRtxPass.cpp
        src/lisa/render/BilateralPass.h
        src/lisa/render/BilateralPass.cpp

        src/lisa/scene/xml.h
        src/lisa/scene/xml.cpp
        src/lisa/scene/gltf.h
        src/lisa/scene/gltf.cpp

        src/lisa/systems/ecs/Registry.h
        src/lisa/systems/ecs/Registry.cpp
        src/lisa/systems/ecs/Component.h
        src/lisa/systems/ecs/Component.cpp

        src/lisa/components/context.h
        src/lisa/components/context.cpp
        src/lisa/components/TransformComponent.h
        src/lisa/components/MeshComponent.h
        src/lisa/components/CameraComponent.h
        src/lisa/components/PointLightComponent.h
        src/lisa/components/DirectionalLightComponent.h
        src/lisa/components/MaterialComponent.h
        src/lisa/components/AmbientLightComponent.h

        src/lisa/systems/render/Renderer.h
        src/lisa/systems/render/Renderer.cpp
        src/lisa/systems/render/Rendergraph.h
        src/lisa/systems/render/Rendergraph.cpp
        src/lisa/systems/render/RenderPass.h
        src/lisa/systems/render/RenderPass.cpp
        src/lisa/systems/render/RenderPassRegistry.h
        src/lisa/systems/render/GraphResources.h
        src/lisa/systems/render/ShaderData.h

        src/lisa/systems/resources/Resource.h
        src/lisa/systems/resources/Resource.cpp
        src/lisa/systems/resources/ResourceManager.h
        src/lisa/systems/resources/ResourceManager.cpp

        src/lisa/update/context.h
        src/lisa/update/context.cpp
        src/lisa/update/CameraMovement.h
        src/lisa/update/CameraMovement.cpp

        src/lisa/utils/common.h
        src/lisa/utils/chk.h
        src/lisa/utils/defer.h
        src/lisa/utils/logging.h
        src/lisa/utils/logging.cpp
        src/lisa/utils/xml.h
        src/lisa/utils/xml.cpp
        src/lisa/utils/Registry.h
        src/lisa/utils/Registry.cpp

        src/lisa/window/context.h
        src/lisa/window/context.cpp
        src/lisa/window/events.h
        src/lisa/window/events.cpp
        src/lisa/window/Window.h
        src/lisa/window/Window.cpp
)

# Profiles library ############################################################

add_subdirectory(src/profiles)

# Library target ##############################################################

add_library(lisa_lib OBJECT ${LISA_SOURCES})

target_sources(lisa_lib
        PRIVATE
        FILE_SET HEADERS
        BASE_DIRS src/lisa
)

target_include_directories(lisa_lib
        PUBLIC
        src/lisa
)

target_link_libraries(lisa_lib
        PUBLIC
        boost::boost
        EnTT::EnTT
        fastgltf::fastgltf
        glm::glm
        KTX::ktx
        pugixml::pugixml
        reflectcpp::reflectcpp
        slang::slang
        spdlog::spdlog
        stb::stb
        tinyexr::tinyexr
        tinyobjloader::tinyobjloader
        Vulkan::Vulkan
        GPUOpen::VulkanMemoryAllocator
        VulkanMemoryAllocator-Hpp::VulkanMemoryAllocator-Hpp
        SDL3pp::SDL3pp
        better-enums
        vulkan_profiles
)

target_precompile_headers(lisa_lib PRIVATE src/lisa/pch/pch.h)

# Executable target ###########################################################

add_executable(lisa src/main.cpp)

target_link_libraries(lisa PRIVATE lisa_lib CLI11::CLI11)

if (WIN32)
    target_compile_definitions(lisa PRIVATE SDL_MAIN_HANDLED)
endif ()

install(TARGETS lisa)

# Shaders target ##############################################################

set(SHADER_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/lisa/shaders")
set(SHADER_DST_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")

target_compile_definitions(lisa_lib PUBLIC
        "LISA_SHADER_DIR=\"${SHADER_DST_DIR}\""
)

add_custom_command(TARGET lisa POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${SHADER_SRC_DIR}"
        "${SHADER_DST_DIR}"
        COMMENT "Copying shaders to build directory..."
)
