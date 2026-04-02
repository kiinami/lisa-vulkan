project(
    'SDL3pp',
    'c', 'cpp',
    version: '0.9.0'
)

sdl3pp_inc = include_directories('include')

sdl3pp_dep = declare_dependency(
    include_directories: sdl3pp_inc
)

import('pkgconfig').generate(
    name: meson.project_name(),
    description: 'A "port" of SDL3 in C++.',
    install_dir: get_option('datadir') / 'pkgconfig',
)

