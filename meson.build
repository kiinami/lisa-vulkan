###############################################################################
# Project definition
###############################################################################
project(
    'lisa', 'cpp',
    version : '0.0.1',
    meson_version: '>=1.8',
    default_options : [
        'cpp_std=c++20',
        'default_library=static'
    ]
)

###############################################################################
# Imports
###############################################################################

fs = import('fs')

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
add_project_arguments('-DGLM_ENABLE_EXPERIMENTAL', language : 'cpp')
add_project_arguments('-DMAGIC_ENUM_RANGE_MIN=0', language : 'cpp')
add_project_arguments('-DMAGIC_ENUM_RANGE_MAX=2000', language : 'cpp')

if get_option('debug')
    add_project_arguments('-DDEBUG', language: 'cpp')
endif

###############################################################################
# Dependencies
###############################################################################
sdk_path = get_option('vulkan_sdk')

if sdk_path == ''
    sdk_path = import('os').getenv('VULKAN_SDK', default: '')
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
    slang = cpp.find_library('slang', dirs: sdk_lib_dirs, required: true)
    vulkan_lib_name = host_machine.system() == 'windows' ? 'vulkan-1' : 'vulkan'
    vulkan = cpp.find_library(vulkan_lib_name, dirs: sdk_lib_dirs, required: true)
else
    slang = dependency('slang', required: true)
    vulkan = dependency('vulkan', required: true)
endif

ktx_path = get_option('ktx_path')

if ktx_path != ''
    ktx = cpp.find_library('ktx', dirs: ktx_path / 'lib', required: true)
    ktx_inc = include_directories(ktx_path / 'include', is_system: true)
else
    ktx_dep_raw = dependency('ktx', required: true, fallback: ['ktx', 'ktx_dep'])
    ktx = ktx_dep_raw
    ktx_inc = []
endif

boost_multi_index = dependency('boost', required : true, modules : ['multi_index'])
cli11 = dependency('CLI11', required: true)
entt = dependency('entt', required : true)
fastgltf = dependency('fastgltf', required : true)
glm = dependency('glm', required: true)
magic_enum = dependency('magic_enum', required : true)
miniz = dependency('miniz', required : true, default_options: ['default_library=static'])
pugixml = dependency('pugixml', required : true, default_options: ['default_library=static'])
reflectcpp = dependency('reflectcpp', required : true, default_options : ['xml=true'])
sdl3 = dependency('sdl3', required: true, default_options: ['default_library=static'])
sdl3pp = dependency('sdl3pp', required: true)
spdlog = dependency('spdlog', required: true, default_options: ['default_library=static'])
stb = dependency('stb', required : true)
tinyexr = dependency('tinyexr', required : true)
tinyobjloader = dependency('tinyobjloader', required: true)
vma = dependency('VulkanMemoryAllocator', required: true)
vma_hpp = dependency('VulkanMemoryAllocator-Hpp', required: true)

###############################################################################
# Sources
###############################################################################
src = files(
    'src/lisa/build.h',
    'src/lisa/implementations.cpp',

    'src/lisa/graphics/vk/VkObject.h',
    'src/lisa/graphics/vk/VkObject.cpp',
    'src/lisa/graphics/vk/NamedVkObject.h',
    'src/lisa/graphics/vk/NamedVkObject.cpp',
    'src/lisa/graphics/vk/VkEnum.h',
    'src/lisa/graphics/vk/VkEnum.cpp',

    'src/lisa/graphics/buffer/Buffer.h',
    'src/lisa/graphics/buffer/Buffer.cpp',

    'src/lisa/graphics/descriptors/DescriptorAllocator.h',
    'src/lisa/graphics/descriptors/DescriptorAllocator.cpp',
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
    'src/lisa/graphics/images/Sampler.h',
    'src/lisa/graphics/images/Sampler.cpp',

    'src/lisa/graphics/memory/MemoryAllocator.h',
    'src/lisa/graphics/memory/MemoryAllocator.cpp',

    'src/lisa/graphics/pipeline/ShaderModule.h',
    'src/lisa/graphics/pipeline/ShaderModule.cpp',
    'src/lisa/graphics/pipeline/PipelineLayout.h',
    'src/lisa/graphics/pipeline/PipelineLayout.cpp',
    'src/lisa/graphics/pipeline/Pipeline.h',
    'src/lisa/graphics/pipeline/Pipeline.cpp',

    'src/lisa/graphics/swapchain/Surface.h',
    'src/lisa/graphics/swapchain/Surface.cpp',
    'src/lisa/graphics/swapchain/Swapchain.h',
    'src/lisa/graphics/swapchain/Swapchain.cpp',

    'src/lisa/graphics/sync/Fence.h',
    'src/lisa/graphics/sync/Fence.cpp',
    'src/lisa/graphics/sync/Semaphore.h',
    'src/lisa/graphics/sync/Semaphore.cpp',

    'src/lisa/graphics/context.h',
    'src/lisa/graphics/context.cpp',

    'src/lisa/resources/context.h',
    'src/lisa/resources/context.cpp',
    'src/lisa/resources/Mesh.h',
    'src/lisa/resources/Mesh.cpp',
    'src/lisa/resources/Shader.h',
    'src/lisa/resources/Shader.cpp',
    'src/lisa/resources/Texture.h',
    'src/lisa/resources/Texture.cpp',
    'src/lisa/resources/Vertex.h',

    'src/lisa/render/ShaderPass.h',
    'src/lisa/render/ShaderPass.cpp',
    'src/lisa/render/ForwardPass.h',
    'src/lisa/render/ForwardPass.cpp',
    'src/lisa/render/GeometryPass.h',
    'src/lisa/render/GeometryPass.cpp',
    'src/lisa/render/CompositionPass.h',
    'src/lisa/render/CompositionPass.cpp',
    'src/lisa/render/DepthPass.h',
    'src/lisa/render/DepthPass.cpp',
    'src/lisa/render/SSAOPass.h',
    'src/lisa/render/SSAOPass.cpp',
    'src/lisa/render/ConvolutionalPass.h',
    'src/lisa/render/ConvolutionalPass.cpp',

    'src/lisa/scene/xml.h',
    'src/lisa/scene/xml.cpp',
    'src/lisa/scene/gltf.h',
    'src/lisa/scene/gltf.cpp',

    'src/lisa/systems/ecs/Registry.h',
    'src/lisa/systems/ecs/Registry.cpp',
    'src/lisa/systems/ecs/Component.h',
    'src/lisa/systems/ecs/Component.cpp',

    'src/lisa/components/context.h',
    'src/lisa/components/context.cpp',
    'src/lisa/components/TransformComponent.h',
    'src/lisa/components/MeshComponent.h',
    'src/lisa/components/CameraComponent.h',
    'src/lisa/components/PointLightComponent.h',
    'src/lisa/components/DirectionalLightComponent.h',
    'src/lisa/components/MaterialComponent.h',
    'src/lisa/components/AmbientLightComponent.h',

    'src/lisa/systems/render/Renderer.h',
    'src/lisa/systems/render/Renderer.cpp',
    'src/lisa/systems/render/Rendergraph.h',
    'src/lisa/systems/render/Rendergraph.cpp',
    'src/lisa/systems/render/RenderPass.h',
    'src/lisa/systems/render/RenderPass.cpp',
    'src/lisa/systems/render/RenderPassRegistry.h',
    'src/lisa/systems/render/GraphResources.h',
    'src/lisa/systems/render/ShaderData.h',

    'src/lisa/systems/resources/Resource.h',
    'src/lisa/systems/resources/Resource.cpp',
    'src/lisa/systems/resources/ResourceManager.h',
    'src/lisa/systems/resources/ResourceManager.cpp',

    'src/lisa/update/context.h',
    'src/lisa/update/context.cpp',
    'src/lisa/update/CameraMovement.h',
    'src/lisa/update/CameraMovement.cpp',

    'src/lisa/utils/common.h',
    'src/lisa/utils/chk.h',
    'src/lisa/utils/defer.h',
    'src/lisa/utils/logging.h',
    'src/lisa/utils/logging.cpp',
    'src/lisa/utils/xml.h',
    'src/lisa/utils/xml.cpp',
    'src/lisa/utils/Registry.h',
    'src/lisa/utils/Registry.cpp',

    'src/lisa/window/context.h',
    'src/lisa/window/context.cpp',
    'src/lisa/window/events.h',
    'src/lisa/window/events.cpp',
    'src/lisa/window/Window.h',
    'src/lisa/window/Window.cpp',
)
inc = include_directories('src/lisa')

###############################################################################
# Shaders
###############################################################################
subdir('src/lisa/shaders')

###############################################################################
# Library definition
###############################################################################
lisa_library = static_library(
    'lisa',
    sources: src,
    include_directories: [inc, sdk_inc, ktx_inc],
    dependencies : [boost_multi_index, entt, fastgltf, glm, ktx, magic_enum, miniz, pugixml, reflectcpp, sdl3, sdl3pp, shader_dep, slang, spdlog, stb, tinyexr, tinyobjloader, vulkan, vma, vma_hpp],
    cpp_pch: 'src/lisa/pch/pch.h'
)

lisa_dep = declare_dependency(
    include_directories: [include_directories('src/lisa'), sdk_inc, ktx_inc],
    link_whole: lisa_library,
    dependencies : [boost_multi_index, entt, fastgltf, glm, ktx, magic_enum, miniz, pugixml, reflectcpp, sdl3, sdl3pp, slang, spdlog, stb, tinyexr, tinyobjloader, vulkan, vma, vma_hpp]
)

###############################################################################
# Executable
###############################################################################
lisa = executable(
    'lisa_app',
    'src/main.cpp',
    install: true,
    dependencies: [cli11, lisa_dep],
    cpp_args: host_machine.system() == 'windows' ? ['-DSDL_MAIN_HANDLED'] : []
)