# lisa-vulkan

Hobby rendering engine in Vulkan

## Features

- Support for Linux and Windows (experimental)
- Full Vulkan 1.4 setup with validation layers using Vulkan Profiles
- ECS architecture with ENTT
- Easy-to-edit scene definition in XML with schema validation
- Multi-pass rendering
- Easy-to-edit XML render graph definitions with schema validation, automatic barrier management and no recompilation needed!
- Many preprocessing, forward rendering, deferred rendering and postprocess passes
- PBR materials, defined by constants or textures
- Multiple light types
- Strong assets pipeline

## Building

### Linux

1. Install dependencies:

### Windows

The engine was primarily implemented in Linux, but adaptations have been made so it runs also in Windows. However, it is recommended to run the engine in Linux if possible.

1. Download and install the following dependencies:
   - [Visual Studio](https://visualstudio.microsoft.com/vs/community/) with "Desktop development with C++" option enabled
   - Python (`uv` is encouraged)
   - [Meson](https://mesonbuild.com/Getting-meson.html)
   - Ninja (`uv tool install ninja`/`pip install ninja`)
   - [LunarG Vulkan SDK](https://vulkan.lunarg.com/sdk/home#windows) (and note the installation directory path)
   - [KTX](https://github.com/KhronosGroup/KTX-Software/releases) (and note the installation directory path)
2. Run the following command to set up the build directory
   ```shell
   meson setup -Dvulkan_sdk=<vulkan SDK path> "-Dktx_path<KTX path>"  --default-library=static buildDir
   ```
3. Compile the app:
   ```shell
   cd buildDir
   meson compile lisa_app -j 10
   ```
4. Now you can run the engine! Refer to the usage section.

> NOTE: you may need to run commands 2, 3 and 4 inside the Developer Command Prompt for VS so that the dependencies are found correctly

## Usage

```
lisa_app [OPTIONS] scene


POSITIONALS:
  scene TEXT:FILE REQUIRED    The XML scene file

OPTIONS:
  -h,     --help              Print this help message and exit
  -g,     --graph TEXT:FILE   The XML rendergraph file
  -l,     --log-level TEXT:{trace,debug,info,warning,error,critical} 
                              The logging level of the application
  -d,     --device INT        The GPU device to use
```

## Bundled scenes

The project comes with a few scenes to test the engine:

- `scenes/lighting_test/lighting_test.xml` - Tests with different lights in different positions
- `scenes/material_test/material_test.xml` - Tests loading of materials using textures
- `scenes/ssao_test/ssao_test.xml` - Tests correct SSAO generation in a way that is easily visible
- `scene/cat/cat.xml` - Joins all features in a single simple scenes
