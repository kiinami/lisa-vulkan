# CMakeLists.txt

cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

###############################################################################
# Project Setup
###############################################################################

if(NOT CMAKE_TOOLCHAIN_FILE AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/vcpkg/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/external/vcpkg/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "Vcpkg toolchain file")
endif()

project(lisa LANGUAGES C CXX)

# Set output directory for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

###############################################################################
# Dependencies
###############################################################################

find_package(Vulkan REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Ktx CONFIG REQUIRED)
find_package(tinyobjloader CONFIG REQUIRED)
find_package(volk CONFIG REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(SDL3 CONFIG REQUIRED)
find_package(slang CONFIG REQUIRED)
find_package(CLI11 CONFIG REQUIRED)
find_package(quill CONFIG REQUIRED)

###############################################################################
# Library
###############################################################################

include_directories(src)
file(GLOB_RECURSE LISA_FILES
        "src/lisa/*.h"
        "src/lisa/*.cpp"
)

add_library(${PROJECT_NAME} STATIC ${LISA_FILES})

# Precompile massive headers to drastically speed up build times
target_precompile_headers(${PROJECT_NAME} PUBLIC
        <vulkan/vulkan.hpp>
        <vk_mem_alloc.h>
        <glm/glm.hpp>
        <SDL3/SDL.h>
)

###############################################################################
# Compiler configuration
###############################################################################

# C++ Standard
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

# Defines (Preprocessor macros)
target_compile_definitions(${PROJECT_NAME} PUBLIC
        VK_NO_PROTOTYPES
        VULKAN_HPP_NO_CONSTRUCTORS
        VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
        NOMINMAX
        _USE_MATH_DEFINES
)

# Platform specific defines
if (WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
            VK_USE_PLATFORM_WIN32_KHR
            _CRT_SECURE_NO_WARNINGS
    )
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
            VK_USE_PLATFORM_WAYLAND_KHR
    )
endif ()

# Compiler Options (Flags)
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /EHsc)
endif ()

###############################################################################
# Linking
###############################################################################

target_link_libraries(${PROJECT_NAME} PRIVATE
        ${VCPKG_VULKAN_LIB}
        KTX::ktx
        SDL3::SDL3
        slang::slang
        glm::glm
        tinyobjloader::tinyobjloader
        volk::volk
        GPUOpen::VulkanMemoryAllocator
        CLI11::CLI11
        quill::quill
)

###############################################################################
# Executable
###############################################################################

add_executable(${PROJECT_NAME}_app src/main.cpp)
target_link_libraries(${PROJECT_NAME}_app PRIVATE
        ${PROJECT_NAME}
        ${VCPKG_VULKAN_LIB}
        KTX::ktx
        SDL3::SDL3
        slang::slang
        glm::glm
        tinyobjloader::tinyobjloader
        volk::volk
        GPUOpen::VulkanMemoryAllocator
        CLI11::CLI11
        quill::quill
)

###############################################################################
# Utility
###############################################################################

# Ensure the debugger runs from the project root so it finds "assets/"
set_target_properties(${PROJECT_NAME} PROPERTIES
        DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Ensure correct debugging
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -g -O1")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fsanitize=address,undefined")
