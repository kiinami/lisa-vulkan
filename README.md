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

## Running

### Windows

1. Download and install the following dependencies:
    - [Meson](https://mesonbuild.com/Getting-meson.html)
    - [LunarG Vulkan SDK](https://vulkan.lunarg.com/sdk/home#windows) (and note the installation directory path)
    - [KTX](https://github.com/KhronosGroup/KTX-Software/releases) (and note the installation directory path)
    - [MSYS2](https://www.msys2.org/) and add `C:\msys64\ucrt64\bin` to the system PATH
2. Run `pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-gcc-libs` on the MSYS2 UCRT64 terminal to get needed dependencies
3. Run the following command to set up the build directory
    ```shell
    meson setup -Dvulkan_sdk=<vulkan SDK path> "-Dktx_path<KTX path>" buildDir
    ```
4. Copy `C:\msys64\ucrt64\bin\libstdc++-6.dll`, `C:\msys64\ucrt64\bin\libwinpthread-1.dl` and `C:\msys64\ucrt64\bin\libgcc_s_seh-1.dll` to `buildDir/`
5. 