# lisa-vulkan

Hobby rendering engine in Vulkan

## Features

- Support for Linux and Windows (experimental)
- Full Vulkan 1.4 setup with validation layers using Vulkan Profiles
- ECS architecture with ENTT
- Easy-to-edit scene definition in XML with schema validation
- Multi-pass rendering
- Easy-to-edit XML render graph definitions with schema validation, automatic barrier management and no recompilation
  needed!
- Many preprocessing, forward rendering, deferred rendering and postprocess passes
- PBR materials, defined by constants or textures
- Multiple light types
- Strong assets pipeline
- FPS-style camera movement and rotation with keyboard and mouse
- Robust debugging framework, with logging, validation layers, debug markers and names, and more

## Building

### Linux

The build process has only been tested in Fedora 43. Other distros will probably work but may require some adjustments.

1. Install dependencies:
    - From DNF:
      `git cmake gcc gcc-c++ ninja ccache freetype-devel harfbuzz-devel libpng-devel libtiff-devel libwebp-devel libvorbis-devel opus-devel opusfile-devel flac-devel mpg123-devel libXcursor-devel libXfixes-devel libXi-devel libXrandr-devel libXScrnSaver-devel libXext-devel mesa-vulkan-drivers vulkan-loader-devel vulkan-validation-layers-devel libpng-static flac`
    - [Conan2](https://docs.conan.io/2/installation.html) (`pip install conan` or `uv tool install conan`)
    - [LunarG Vulkan SDK](https://vulkan.lunarg.com/sdk/home#windows) (and note the installation directory path
2. Clone the repository:
    ```shell
    git clone https://github.com/kiinami/lisa-vulkan.git
    cd lisa-vulkan
    ```
3. Get the dependencies using Conan (if this is your first time using Conan, grab a coffee, it may take a while to
   download and build everything):
    ```shell
    conan install . --build=missing --profile=conan/profiles/debug
    ```
4. Run the following command to set up the build directory
   ```shell
   cmake -DCMAKE_BUILD_TYPE=Debug --preset debug -S . -B ./build/Debug -DVULKAN_SDK=<vulkan SDK path>
   ```
5. Compile the app:
   ```shell
    cmake --build build/Debug --target lisa -j 10
6. Now you can run the engine! Refer to the usage section.

---

1. Install dependencies:
    - [LunarG Vulkan SDK](https://vulkan.lunarg.com/sdk/home#windows) (and note the installation directory path)
    - [Boost](https://www.boost.org/releases/latest/)
2. Run the following command to set up the build directory
   ```shell
   meson setup -Dvulkan_sdk=<vulkan SDK path>
   ```
3. Compile the app:
   ```shell
   cd buildDir
   meson compile lisa_app -j 10
   ```
4. Now you can run the engine! Refer to the usage section.

### Windows

The engine was primarily implemented in Linux, but adaptations have been made so it runs also in Windows. However, it is
recommended to run the engine in Linux if possible.

1. Download and install the following dependencies:
    - [Visual Studio](https://visualstudio.microsoft.com/vs/community/) with "Desktop development with C++" option
      enabled
    - Python (`uv` is encouraged)
    - [Meson](https://mesonbuild.com/Getting-meson.html)
    - Ninja (`uv tool install ninja`/`pip install ninja`)
    - [LunarG Vulkan SDK](https://vulkan.lunarg.com/sdk/home#windows) (and note the installation directory path)
    - [KTX](https://github.com/KhronosGroup/KTX-Software/releases) (and note the installation directory path)
    - [Boost](https://www.boost.org/releases/latest/)

> NOTE: you may need to run commands 2, 3 and 4 inside the Developer Command Prompt for VS so that the dependencies are
> found correctly

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

## Usage

```shell
$ lisa_app --help

lisa_app [OPTIONS] scene


POSITIONALS:
  scene TEXT:FILE REQUIRED    The XML scene file

OPTIONS:
  -h,     --help              Print this help message and exit
  -g,     --graph TEXT:FILE   The XML rendergraph file
  -l,     --log-level TEXT:{trace,debug,info,warning,error,critical} 
                              The logging level of the application
          --width INT         The initial window width
          --height INT        The initial window height
  -d,     --device INT        The GPU device to use
```

### Controls

WASD: movement
Q and E: movement up and down
Left click: move camera around

## Bundled scenes

The project comes with a few scenes to test the engine:

- `scenes/lighting_test/lighting_test.xml` - Tests with different lights in different positions
- `scenes/material_test/material_test.xml` - Tests loading of materials using textures
- `scenes/ssao_test/ssao_test.xml` - Tests correct SSAO generation in a way that is easily visible (use with
  `test_ssao.xml` render graph)
- `scene/cat/cat.xml` - Joins all features in a single simple scenes

It also comes with a few predefined render graphs in `assets/rendergraphs`:

- `forward.xml` - forwards pipeline, very simple
- `deferred.xml` - deferred pipeline with SSAO
- `ssao_test.xml` - special render graph to test SSAO
