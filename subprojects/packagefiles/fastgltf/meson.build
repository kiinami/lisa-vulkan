project(
  'fastgltf',
  'c', 'cpp',
  version : '0.9.0',
  default_options : ['cpp_std=c++17'],
)

# ─── Options ────────────────────────────────────────────────────────────────

opt_use_custom_smallvector = get_option('use_custom_smallvector')
opt_disable_custom_memory_pool = get_option('disable_custom_memory_pool')
opt_use_64bit_float = get_option('use_64bit_float')
opt_compile_as_cpp20 = get_option('compile_as_cpp20')
opt_enable_khr_implicit_shapes = get_option('enable_khr_implicit_shapes')
opt_enable_khr_physics_rigid_bodies = get_option('enable_khr_physics_rigid_bodies')

# KHR_physics_rigid_bodies implies KHR_implicit_shapes
if opt_enable_khr_physics_rigid_bodies
  opt_enable_khr_implicit_shapes = true
endif

# ─── C++ standard ───────────────────────────────────────────────────────────

cpp = meson.get_compiler('cpp')

cpp_std = opt_compile_as_cpp20 ? 'c++20' : 'c++17'
add_project_arguments('-std=' + cpp_std, language : 'cpp')

# ─── Dependencies ───────────────────────────────────────────────────────────

simdjson_dep = dependency('simdjson', required : true)

# Old GCC (<9.1) and old Clang (<9.0) need an explicit filesystem library.
compiler_id = cpp.get_id()
compiler_version = cpp.version()

fs_link_args = []
if compiler_id == 'gcc' and compiler_version.version_compare('<9.1')
  fs_link_args = ['-lstdc++fs']
elif compiler_id == 'clang' and compiler_version.version_compare('<9.0')
  fs_link_args = ['-lc++fs']
endif

android_dep = []
if host_machine.system() == 'android'
  android_dep = [cpp.find_library('android')]
endif

# ─── Public compile definitions ─────────────────────────────────────────────

fastgltf_compile_defs = [
  '-DFASTGLTF_USE_CUSTOM_SMALLVECTOR=' + (opt_use_custom_smallvector ? '1' : '0'),
  '-DFASTGLTF_DISABLE_CUSTOM_MEMORY_POOL=' + (opt_disable_custom_memory_pool ? '1' : '0'),
  '-DFASTGLTF_USE_64BIT_FLOAT=' + (opt_use_64bit_float ? '1' : '0'),
  '-DFASTGLTF_ENABLE_KHR_IMPLICIT_SHAPES=' + (opt_enable_khr_implicit_shapes ? '1' : '0'),
  '-DFASTGLTF_ENABLE_KHR_PHYSICS_RIGID_BODIES=' + (opt_enable_khr_physics_rigid_bodies ? '1' : '0'),
]

# ─── Library ────────────────────────────────────────────────────────────────

fastgltf_headers = files(
  'include/fastgltf/base64.hpp',
  'include/fastgltf/core.hpp',
  'include/fastgltf/dxmath_element_traits.hpp',
  'include/fastgltf/glm_element_traits.hpp',
  'include/fastgltf/tools.hpp',
  'include/fastgltf/types.hpp',
  'include/fastgltf/util.hpp',
  'include/fastgltf/math.hpp',
)

fastgltf_sources = files(
  'src/fastgltf.cpp',
  'src/base64.cpp',
  'src/io.cpp',
)

fastgltf_lib = library(
  'fastgltf',
  fastgltf_sources,
  include_directories : include_directories('include'),
  cpp_args : fastgltf_compile_defs,
  link_args : fs_link_args,
  dependencies : [simdjson_dep] + android_dep,
  version : meson.project_version(),
  install : true,
)

# ─── Declared dependency (for use as a subproject) ──────────────────────────

fastgltf_dep = declare_dependency(
  link_with : fastgltf_lib,
  include_directories : include_directories('include'),
  compile_args : fastgltf_compile_defs,
)

meson.override_dependency('fastgltf', fastgltf_dep)

# ─── Install headers ────────────────────────────────────────────────────────

install_headers(fastgltf_headers, subdir : 'fastgltf')

# ─── pkg-config ─────────────────────────────────────────────────────────────

import('pkgconfig').generate(
  fastgltf_lib,
  name : 'fastgltf',
  description : 'A fast glTF 2.0 parser and writer library',
  version : meson.project_version(),
  subdirs : 'fastgltf',
)
