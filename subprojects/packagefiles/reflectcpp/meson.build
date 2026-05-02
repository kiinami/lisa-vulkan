project(
  'reflectcpp',
  'cpp',
  version : '0.24.0',
  default_options : ['cpp_std=c++20'],
)

# ─── Options ────────────────────────────────────────────────────────────────

opt_shared = get_option('build_shared')
opt_strict_warnings = get_option('strict_warnings')
opt_use_std_expected = get_option('use_std_expected')

opt_all_formats = get_option('all_formats')

opt_json = get_option('json') or opt_all_formats
opt_avro = get_option('avro') or opt_all_formats
opt_bson = get_option('bson') or opt_all_formats
opt_capnproto = get_option('capnproto') or opt_all_formats
opt_cbor = get_option('cbor') or opt_all_formats
opt_cereal = get_option('cereal') or opt_all_formats
opt_csv = get_option('csv') or opt_all_formats
opt_flexbuffers = get_option('flexbuffers') or opt_all_formats
opt_msgpack = get_option('msgpack') or opt_all_formats
opt_parquet = get_option('parquet') or opt_all_formats
opt_xml = get_option('xml') or opt_all_formats
opt_toml = get_option('toml') or opt_all_formats
opt_ubjson = get_option('ubjson') or opt_all_formats
opt_yaml = get_option('yaml') or opt_all_formats
opt_yas = get_option('yas') or opt_all_formats
opt_boost_serialization = get_option('boost_serialization') or opt_all_formats

# JSON implementation (yyjson) is shared by: json, avro, cbor, ubjson
needs_json_impl = opt_json or opt_avro or opt_cbor or opt_ubjson

# ─── C++ standard ───────────────────────────────────────────────────────────

# use_std_expected requires C++23; bump the standard if requested.
# The project default_options already sets c++20; override here if needed.
if opt_use_std_expected
  add_project_arguments('-std=c++23', language : 'cpp')
endif

# ─── Compiler flags ─────────────────────────────────────────────────────────

cpp = meson.get_compiler('cpp')

if opt_strict_warnings
  if cpp.get_id() == 'msvc'
    add_project_arguments('/W4', language : 'cpp')
  else
    add_project_arguments(
      '-Wall', '-Wextra', '-Wpedantic', '-Wshadow', '-Wconversion',
      language : 'cpp',
    )
  endif
endif

# ─── Sources & dependency accumulators ──────────────────────────────────────

reflectcpp_sources = files('src/reflectcpp.cpp')
reflectcpp_deps = []
reflectcpp_compile_args = []
reflectcpp_inc = [include_directories('include')]

# ─── ctre (always required; replaces the bundled copy) ──────────────────────

reflectcpp_deps += dependency('ctre', required : true)

# ─── yyjson (JSON / AVRO / CBOR / UBJSON backend) ───────────────────────────

if needs_json_impl
  reflectcpp_sources += files('src/reflectcpp_json.cpp')
  reflectcpp_deps += dependency('yyjson', required : true)
endif

# ─── Per-format backends ────────────────────────────────────────────────────

if opt_avro
  reflectcpp_sources += files('src/reflectcpp_avro.cpp')
  reflectcpp_deps += dependency('avro-c', required : true)
  reflectcpp_deps += dependency('jansson', required : true)
endif

if opt_bson
  reflectcpp_sources += files('src/reflectcpp_bson.cpp')
  # mongo-c-driver exposes libbson via libbson-1.0
  reflectcpp_deps += dependency('libbson-1.0', required : true)
endif

if opt_capnproto
  reflectcpp_sources += files('src/reflectcpp_capnproto.cpp')
  reflectcpp_deps += dependency('kj', required : true)
  reflectcpp_deps += dependency('capnp', required : true)
  reflectcpp_deps += dependency('capnpc', required : true)
  reflectcpp_deps += dependency('kj-gzip', required : true)
endif

# jsoncons is header-only and shared by CBOR and UBJSON; find it once.
_jsoncons_dep = disabler()
if opt_cbor or opt_ubjson
  _jsoncons_dep = dependency('jsoncons', required : true)
  reflectcpp_deps += _jsoncons_dep
endif

if opt_cbor
  reflectcpp_sources += files('src/reflectcpp_cbor.cpp')
endif

if opt_cereal
  reflectcpp_sources += files('src/reflectcpp_cereal.cpp')
  reflectcpp_deps += dependency('cereal', required : true)
endif

# Arrow is shared by CSV and Parquet; find it once.
_arrow_dep = disabler()
if opt_csv or opt_parquet
  _arrow_dep = dependency('arrow', required : true)
  reflectcpp_deps += _arrow_dep
endif

if opt_csv
  # CSV support uses Arrow only — no extra source file in the CMake build
endif

if opt_flexbuffers
  reflectcpp_sources += files('src/reflectcpp_flexbuf.cpp')
  reflectcpp_deps += dependency('flatbuffers', required : true)
endif

if opt_msgpack
  reflectcpp_sources += files('src/reflectcpp_msgpack.cpp')
  # The library ships under several pkg-config names; try them in order.
  _msgpack_dep = dependency('msgpack-c', required : false)
  if not _msgpack_dep.found()
    _msgpack_dep = dependency('msgpackc', required : false)
  endif
  if not _msgpack_dep.found()
    _msgpack_dep = dependency('msgpack', required : true)
  endif
  reflectcpp_deps += _msgpack_dep
endif

if opt_parquet
  reflectcpp_deps += dependency('parquet', required : true)
endif

if opt_toml
  reflectcpp_sources += files('src/reflectcpp_toml.cpp')
  reflectcpp_deps += dependency('tomlplusplus', required : true)
endif

if opt_ubjson
  reflectcpp_sources += files('src/reflectcpp_ubjson.cpp')
endif

if opt_xml
  reflectcpp_sources += files('src/reflectcpp_xml.cpp')
  reflectcpp_deps += dependency('pugixml', required : true)
endif

if opt_yaml
  reflectcpp_sources += files('src/reflectcpp_yaml.cpp')
  reflectcpp_deps += dependency('yaml-cpp', required : true)
endif

if opt_boost_serialization
  reflectcpp_sources += files('src/reflectcpp_boost_serialization.cpp')
  reflectcpp_deps += dependency('boost', modules : ['serialization'], required : true)
endif

if opt_yas
  reflectcpp_sources += files('src/reflectcpp_yas.cpp')
  # YAS is header-only with no pkg-config file; the user supplies the path
  # via -Dyas_include_dir=<path> or by placing it on the system include path.
  yas_inc_dir = get_option('yas_include_dir')
  if yas_inc_dir != ''
    reflectcpp_inc += include_directories(yas_inc_dir)
  endif
endif

# ─── Compile definitions ────────────────────────────────────────────────────

if opt_use_std_expected
  reflectcpp_compile_args += '-DREFLECTCPP_USE_STD_EXPECTED'
endif

if opt_shared
  reflectcpp_compile_args += '-DRFL_BUILD_SHARED'
endif

# ─── Library target ─────────────────────────────────────────────────────────

reflectcpp_lib = library(
  'reflectcpp',
  reflectcpp_sources,
  include_directories : reflectcpp_inc,
  dependencies : reflectcpp_deps,
  cpp_args : reflectcpp_compile_args,
  version : meson.project_version(),
  install : true,
)

# ─── Declared dependency (for use as a Meson subproject) ────────────────────

reflectcpp_dep = declare_dependency(
  link_with : reflectcpp_lib,
  include_directories : reflectcpp_inc,
  compile_args : reflectcpp_compile_args,
  dependencies : reflectcpp_deps,
)

meson.override_dependency('reflectcpp', reflectcpp_dep)

# ─── Install headers ────────────────────────────────────────────────────────

install_subdir('include', install_dir : get_option('includedir'), strip_directory : true)

# ─── pkg-config ─────────────────────────────────────────────────────────────

import('pkgconfig').generate(
  reflectcpp_lib,
  name : 'reflectcpp',
  description : 'C++20 compile-time reflection and serialization library',
  version : meson.project_version(),
)