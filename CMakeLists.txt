# CMakeLists.txt

cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

###############################################################################
# Project Setup
###############################################################################

project(lisa LANGUAGES C CXX)

# Add custom cmake module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Set output directory for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

###############################################################################
# Dependencies
###############################################################################

# Vulkan
find_package(Vulkan REQUIRED)
message(STATUS "Found Vulkan: ${Vulkan_LIBRARY}")

# 2. GLM (Math)
find_package(glm QUIET)
if (NOT glm_FOUND)
    include(FetchContent)
    FetchContent_Declare(
            glm
            GIT_REPOSITORY https://github.com/g-truc/glm
            GIT_TAG 1.0.1 # Pinning to a specific tag is safer than '1.0'
            GIT_SHALLOW ON
            SYSTEM         # Suppress warnings from external lib
    )
    FetchContent_MakeAvailable(glm)
endif ()

# KTX
add_library(ktx STATIC
        external/ktx/lib/texture.c
        external/ktx/lib/hashlist.c
        external/ktx/lib/checkheader.c
        external/ktx/lib/swap.c
        external/ktx/lib/memstream.c
        external/ktx/lib/filestream.c
        external/ktx/lib/vkloader.c
)

target_include_directories(ktx PUBLIC
        external/ktx/include
        external/ktx/other_include
)

target_link_libraries(ktx PRIVATE Vulkan::Vulkan)

# SDL3 & Slang
find_library(SDL_LIBRARY
        NAMES SDL3
        HINTS "$ENV{VULKAN_SDK}/lib"
        REQUIRED
)

find_library(SLANG_LIBRARY
        NAMES slang
        HINTS "$ENV{VULKAN_SDK}/lib"
        REQUIRED
)

###############################################################################
# Executable target
###############################################################################

add_executable(${PROJECT_NAME}
        src/lisa/main.cpp

        assets/shader.slang
)

###############################################################################
# Compiler configuration
###############################################################################

# C++ Standard
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

# Include Directories
target_include_directories(${PROJECT_NAME} PRIVATE
        ${Vulkan_INCLUDE_DIRS}
        external/tinyobj
)

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
        ktx
        ${SDL_LIBRARY}
        ${SLANG_LIBRARY}
        glm::glm # Works for both found package and FetchContent
)

###############################################################################
# Utility
###############################################################################

# Ensure the debugger runs from the project root so it finds "assets/"
set_target_properties(${PROJECT_NAME} PROPERTIES
        DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
