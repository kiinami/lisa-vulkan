###############################################################################
# Project definition
###############################################################################
project(
    'lisa', 'cpp',
    version : '0.0.1',
    meson_version: '>=1.8',
    default_options : ['cpp_std=c++20']
)

###############################################################################
# Project arguments
###############################################################################

add_project_arguments('-DNOMINMAX', language: 'cpp')
add_project_arguments('-D_USE_MATH_DEFINES', language: 'cpp')
add_project_arguments('-DVULKAN_HPP_NO_CONSTRUCTORS', language: 'cpp')
add_project_arguments('-DVULKAN_HPP_NO_STRUCT_CONSTRUCTORS', language: 'cpp')
add_project_arguments('-DGLM_FORCE_RADIANS', language: 'cpp')
add_project_arguments('-DGLM_FORCE_DEPTH_ZERO_TO_ONE', language: 'cpp')

###############################################################################
# Dependencies
###############################################################################
sdk_path = get_option('vulkan_sdk')

if sdk_path == ''
    sdk_path = import('os').getenv('VULKAN_SDK', default: '')
endif

if sdk_path != ''
    add_project_arguments('-I' + sdk_path / 'include', language: 'cpp')
    add_project_link_arguments('-L' + sdk_path / 'lib', language: 'cpp')
endif

sdk_inc = []
sdk_lib_dirs = []
if sdk_path != ''
    sdk_inc = include_directories(sdk_path / 'include', is_system: true)
    sdk_lib_dirs = [sdk_path / 'lib']
    message('Using Vulkan SDK at: ' + sdk_path)
endif


cli11 = dependency('CLI11', required: true)
glm = dependency('glm', required: true)
sdl3 = dependency('sdl3', required: true)
slang = dependency('slang', required: true)
spdlog = dependency('spdlog', required: true)
tinyobjloader = dependency('tinyobjloader', required: true)
vulkan = dependency('vulkan', required: true)
vulkan_memory_allocator = dependency('VulkanMemoryAllocator', required: true)

###############################################################################
# Sources
###############################################################################
src = files(
    'src/lisa/utils/common.h',
    'src/lisa/implementations.cpp',

    'src/lisa/graphics/context.h',
    'src/lisa/graphics/context.cpp',
    'src/lisa/graphics/utils.h',
    'src/lisa/graphics/utils.cpp',

    'src/lisa/graphics/commands/Queue.h',
    'src/lisa/graphics/commands/Queue.cpp',

    'src/lisa/graphics/device/Instance.h',
    'src/lisa/graphics/device/Instance.cpp',
    'src/lisa/graphics/device/PhysicalDevice.h',
    'src/lisa/graphics/device/PhysicalDevice.cpp',
    'src/lisa/graphics/device/LogicalDevice.h',
    'src/lisa/graphics/device/LogicalDevice.cpp',

    'src/lisa/graphics/vk/vk.h',
    'src/lisa/graphics/vk/vk.cpp',

    'src/lisa/utils/chk.h',
    'src/lisa/utils/defer.h',
    'src/lisa/utils/logging.h',
    'src/lisa/utils/logging.cpp'
)
inc = include_directories('src/lisa')

###############################################################################
# Library definition
###############################################################################
lisa_library = library(
    'lisa',
    sources: src,
    include_directories: inc,
    dependencies: [glm, sdl3, slang, spdlog, tinyobjloader, vulkan, vulkan_memory_allocator],
    cpp_pch: 'src/lisa/pch/pch.h'
)

lisa_dep = declare_dependency(
    include_directories: include_directories('src/lisa'),
    link_with: lisa_library,
    dependencies: [glm, sdl3, slang, spdlog, tinyobjloader, vulkan, vulkan_memory_allocator]
)

###############################################################################
# Executable
###############################################################################
lisa = executable(
    'lisa_app',
    'src/main.cpp',
    install: true,
    dependencies: [cli11, lisa_dep]
)
