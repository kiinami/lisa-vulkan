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
add_project_arguments('-DVP_USE_OBJECT', language: 'cpp')
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


cpp = meson.get_compiler('cpp')

if sdk_path != ''
    slang = cpp.find_library('slang', dirs: sdk_path / 'lib', required: true)
    vulkan = cpp.find_library('vulkan', dirs: sdk_path / 'lib', required: true)
else
    slang = dependency('slang', required: true)
    vulkan = dependency('vulkan', required: true)
endif

cli11 = dependency('CLI11', required: true)
glm = dependency('glm', required: true)
ktx = dependency('ktx', required: true)
sdl3 = dependency('sdl3', required: true)
sdl3pp = dependency('sdl3pp', required: true)
spdlog = dependency('spdlog', required: true)
tinyobjloader = dependency('tinyobjloader', required: true)
vma = dependency('VulkanMemoryAllocator', required: true)
vma_hpp = dependency('VulkanMemoryAllocator-Hpp', required: true)

###############################################################################
# Sources
###############################################################################
src = files(
    'src/lisa/utils/common.h',
    'src/lisa/implementations.cpp',

    'src/lisa/graphics/context.h',
    'src/lisa/graphics/context.cpp',

    'src/lisa/graphics/buffer/Buffer.h',
    'src/lisa/graphics/buffer/Buffer.cpp',

    'src/lisa/graphics/descriptors/DescriptorContainer.h',
    'src/lisa/graphics/descriptors/DescriptorContainer.cpp',

    'src/lisa/graphics/device/Instance.h',
    'src/lisa/graphics/device/Instance.cpp',
    'src/lisa/graphics/device/PhysicalDevice.h',
    'src/lisa/graphics/device/PhysicalDevice.cpp',
    'src/lisa/graphics/device/LogicalDevice.h',
    'src/lisa/graphics/device/LogicalDevice.cpp',

    'src/lisa/graphics/commands/CommandBuffer.h',
    'src/lisa/graphics/commands/CommandBuffer.cpp',

    'src/lisa/graphics/images/Image.h',
    'src/lisa/graphics/images/Image.cpp',
    'src/lisa/graphics/images/ImageFormat.h',
    'src/lisa/graphics/images/ImageFormat.cpp',

    'src/lisa/graphics/memory/MemoryAllocator.h',
    'src/lisa/graphics/memory/MemoryAllocator.cpp',

    'src/lisa/graphics/swapchain/Surface.h',
    'src/lisa/graphics/swapchain/Surface.cpp',
    'src/lisa/graphics/swapchain/Swapchain.h',
    'src/lisa/graphics/swapchain/Swapchain.cpp',

    'src/lisa/resources/constants.h',
    'src/lisa/resources/Mesh.h',
    'src/lisa/resources/Mesh.cpp',
    'src/lisa/resources/Shader.h',
    'src/lisa/resources/Shader.cpp',
    'src/lisa/resources/Texture.h',
    'src/lisa/resources/Texture.cpp',
    'src/lisa/resources/Vertex.h',

    'src/lisa/systems/render/Renderer.h',
    'src/lisa/systems/render/Renderer.cpp',
    'src/lisa/systems/render/Rendergraph.h',
    'src/lisa/systems/render/Rendergraph.cpp',
    'src/lisa/systems/render/RenderPass.h',
    'src/lisa/systems/render/RenderPass.cpp',
    'src/lisa/systems/render/RenderResource.h',
    'src/lisa/systems/render/RenderResource.cpp',

    'src/lisa/systems/resources/Resource.h',
    'src/lisa/systems/resources/Resource.cpp',
    'src/lisa/systems/resources/ResourceHandle.h',
    'src/lisa/systems/resources/ResourceHandle.cpp',
    'src/lisa/systems/resources/ResourceManager.h',
    'src/lisa/systems/resources/ResourceManager.cpp',

    'src/lisa/utils/chk.h',
    'src/lisa/utils/defer.h',
    'src/lisa/utils/logging.h',
    'src/lisa/utils/logging.cpp',

    'src/lisa/window/context.h',
    'src/lisa/window/context.cpp',
    'src/lisa/window/Window.h',
    'src/lisa/window/Window.cpp',
)
inc = include_directories('src/lisa')

###############################################################################
# Library definition
###############################################################################
lisa_library = library(
    'lisa',
    sources: src,
    include_directories: inc,
    dependencies: [glm, ktx, sdl3, sdl3pp, slang, spdlog, tinyobjloader, vulkan, vma, vma_hpp],
    cpp_pch: 'src/lisa/pch/pch.h'
)

lisa_dep = declare_dependency(
    include_directories: include_directories('src/lisa'),
    link_with: lisa_library,
    dependencies: [glm, ktx, sdl3, sdl3pp, slang, spdlog, tinyobjloader, vulkan, vma, vma_hpp]
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

###############################################################################
# Assets
###############################################################################
install_subdir('assets', install_dir: get_option('bindir') / 'assets')

run_command(
    'cp', '-r',
    meson.project_source_root() / 'assets',
    meson.project_build_root() / 'assets',
    check: false
)
