project(
    'SDL3pp',
    'c', 'cpp',
    version: '0.9.3'
)

sdl3pp_inc = include_directories('include')

cpp = meson.get_compiler('cpp')

sdl3pp_dep = declare_dependency(
    include_directories: sdl3pp_inc,
    compile_args : ['-fpermissive']
)

import('pkgconfig').generate(
    name: meson.project_name(),
    description: 'A "port" of SDL3 in C++.',
    install_dir: get_option('datadir') / 'pkgconfig',
)