project(
    'VulkanMemoryAllocator-Hpp',
    'c', 'cpp',
    version: '3.3.0'
)

vma_hpp_inc = include_directories('VulkanMemoryAllocator-Hpp')

vma_hpp_dep = declare_dependency(
    include_directories: vma_hpp_inc
)

import('pkgconfig').generate(
    name: meson.project_name(),
    description: 'VMA-Hpp provides C++ bindings for VulkanMemoryAllocator, consistent and compatible with Vulkan C++ bindings (Vulkan-Hpp).',
    install_dir: get_option('datadir') / 'pkgconfig',
)

