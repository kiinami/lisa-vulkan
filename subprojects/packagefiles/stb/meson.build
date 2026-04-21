project(
  'stb',
  'c', 'cpp',
  version : '31c1ad37456438565541f4919958214b6e762fb4'
)

stb_inc = include_directories('.')

stb_dep = declare_dependency(
  include_directories : stb_inc
)

import('pkgconfig').generate(
  name : meson.project_name(),
  description : 'stb single-file public domain libraries for C/C++',
  install_dir : get_option('datadir') / 'pkgconfig',
)

