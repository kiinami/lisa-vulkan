project(
    'ktx',
    'c', 'cpp',
    version: '4.4.2'
)

cc = meson.get_compiler('c')
ktx_lib = cc.find_library('ktx', dirs: meson.current_source_dir() / 'lib')

ktx_inc = include_directories('include')

ktx_dep = declare_dependency(
    include_directories: ktx_inc,
    dependencies: ktx_lib
)

import('pkgconfig').generate(
    name: meson.project_name(),
    description: 'KTX (Khronos Texture) Library and Tools',
    version: meson.project_version(),
    libraries: ktx_lib,
)