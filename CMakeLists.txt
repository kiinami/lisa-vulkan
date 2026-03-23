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

# Add custom cmake module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

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

###############################################################################
# Executable target
###############################################################################

add_executable(${PROJECT_NAME}
        assets/shaders/shader.slang

        src/lisa/utils/ExternalLibs.cpp
        src/lisa/main.cpp
        src/lisa/core/Application.cpp
        src/lisa/core/Application.h
        src/lisa/utils/chk.cpp
        src/lisa/utils/ExternalLibs.cpp
        src/lisa/core/Renderer.cpp
        src/lisa/core/Renderer.h
        src/lisa/core/VulkanRHI.cpp
        src/lisa/core/VulkanRHI.h
        src/lisa/core/Window.cpp
        src/lisa/core/Window.h
)

###############################################################################
# Compiler configuration
###############################################################################

# C++ Standard
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

# Defines (Preprocessor macros)
target_compile_definitions(${PROJECT_NAME} PRIVATE
        VK_NO_PROTOTYPES
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
        Vulkan::Vulkan
        KTX::ktx
        SDL3::SDL3
        slang::slang
        glm::glm
        tinyobjloader::tinyobjloader
        volk::volk
        GPUOpen::VulkanMemoryAllocator
)

###############################################################################
# Utility
###############################################################################

# Ensure the debugger runs from the project root so it finds "assets/"
set_target_properties(${PROJECT_NAME} PROPERTIES
        DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
