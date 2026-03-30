project(
    'tinyobjloader',
    'c', 'cpp',
    version: '2.0.0-rc13'
)

tinyobjloader_inc = include_directories('.')

tinyobjloader_dep = declare_dependency(
    include_directories: tinyobjloader_inc
)

import('pkgconfig').generate(
    name: meson.project_name(),
    description: 'Tiny but powerful single file wavefront obj loader written in C++11',
    install_dir: get_option('datadir') / 'pkgconfig',
)

