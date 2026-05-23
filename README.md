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

## Project setup

### Linux

The process has only been tested in Fedora 43. Other distros will probably work but may require some adjustments,
especially regarding the dependencies' installation.

1. Install dependencies:
   - From DNF:
     `git cmake gcc gcc-c++ ninja ccache freetype-devel harfbuzz-devel libpng-devel libtiff-devel libwebp-devel libvorbis-devel opus-devel opusfile-devel flac-devel mpg123-devel libXcursor-devel libXfixes-devel libXi-devel libXrandr-devel libXScrnSaver-devel libXext-devel mesa-vulkan-drivers vulkan-loader-devel vulkan-validation-layers-devel libpng-static flac just`
   - [Conan2](https://docs.conan.io/2/installation.html) (`uv tool install conan`/`pip install conan`)
   - [LunarG Vulkan SDK](https://vulkan.lunarg.com/sdk/home#windows) (and note the installation directory path
2. Clone the repository:
    ```shell
    git clone https://github.com/kiinami/lisa-vulkan.git
    cd lisa-vulkan
    ```
3. Copy the `.env.example` file into a new `.env` file and set the `VULKAN_SDK` variable to the installation path you noted in step 1.

You can now continue to the [Building and running](#building-and-running) section.

### Windows

The engine was primarily implemented in Linux, but adaptations have been made so it runs also in Windows. However, it is
recommended to run the engine in Linux if possible.

1. Download and install the following dependencies:
   - [Visual Studio](https://visualstudio.microsoft.com/vs/community/) with "Desktop development with C++" option
     enabled with the following optional features enabled:
      - MSCV Build Tools for x64/x86 (Latest)
      - C++ CMake tools for Windows
      - Windows 11 SDK
      - C++ Clang tools for Windows
   - Python (`uv` is encouraged)
   - Conan (`uv tool install conan`/`pip install conan`)
   - [LunarG Vulkan SDK](https://vulkan.lunarg.com/sdk/home#windows) (and note the installation directory path)
   - [Just](https://just.systems/man/en/installation.html)
2. Clone the repository:
    ```shell
    git clone https://github.com/kiinami/lisa-vulkan.git
    cd lisa-vulkan
    ```
3. Copy the `.env.example` file into a new `.env` file and set the `VULKAN_SDK` variable to the installation path you noted in step 1.

You can now continue to the [Building and running](#building-and-running) section.

## Building and running

1. Get the dependencies using Conan (if this is your first time using Conan, grab a coffee, it may take a while to
   download and build everything):
    ```shell
    just deps
    ```
2. Build the project:
   ```shell
   just build
   ```
3. Run the app:
   ```shell
    just run -- --help
   ```
4. Now you can run the engine! Refer to the [Usage](#usage) section.

For release mode, add the `--release` flag (`-r` is also valid) to all `just` commands: `just deps --release`, `just build -r`, etc. To run the app in release mode, you need to put the flag before the `--`, otherwise it will be parsed as a flag for the app.

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
