project(
  'tinyexr',
  'c', 'cpp',
  version : '1.0.13'
)

tinyexr_inc = include_directories('.')

tinyexr_dep = declare_dependency(
  include_directories : tinyexr_inc
)

import('pkgconfig').generate(
  name : meson.project_name(),
  description : 'Tiny OpenEXR image loader/saver library',
  install_dir : get_option('datadir') / 'pkgconfig',
)

