# Vulkan Context Design Spec

## Overview
This specification details a clean architecture for managing global Vulkan state within the `lisa-vulkan` engine. The goal is to provide easy access to the core Vulkan context (`Instance`, `PhysicalDevice`, `LogicalDevice`, `Queue`s) without passing a multitude of references or pointers around the codebase.

## Design Approach: Static Namespace
The engine will use a static namespace pattern. This approach centralizes the Vulkan context initialization, cleanup, and access.

### 1. Architecture
*   **Location:** A new module `src/lisa/graphics/Context.h` and `src/lisa/graphics/Context.cpp`.
*   **Namespace:** `lisa::graphics::Context`
*   **Visibility:** The header will expose simple accessor functions (e.g., `GetDevice()`). The `.cpp` file will hold the actual static instances or pointers to the Vulkan objects, keeping them hidden from the rest of the application and preventing multiple definitions.

### 2. State Management
The `Context.cpp` file will manage the lifetime of the core Vulkan objects:
*   `std::unique_ptr<Instance> s_Instance`
*   `std::unique_ptr<PhysicalDevice> s_PhysicalDevice`
*   `std::unique_ptr<LogicalDevice> s_LogicalDevice`
*   `Queue* s_GraphicsQueue` (Pointer into the logical device's queues, or managed by the logical device and queried via the Context)

### 3. Accessors
The header will provide globally accessible functions returning references or pointers to the underlying objects. This allows any system (like a Renderer, AssetManager, or CommandBuffer wrapper) to easily access the device:
*   `Instance& GetInstance();`
*   `PhysicalDevice& GetPhysicalDevice();`
*   `LogicalDevice& GetDevice();`
*   `Queue& GetGraphicsQueue();`

### 4. Initialization and Cleanup
Vulkan requires strict ordering of destruction (e.g., Logical Device before Physical Device/Instance). To enforce this:
*   `void Init();` - Called once at application startup (e.g., in `main.cpp` or the core Engine initialization). This function creates the `Instance`, selects the `PhysicalDevice`, and creates the `LogicalDevice`.
*   `void Shutdown();` - Called once at application shutdown. This function destroys the objects in the reverse order of their creation and sets the pointers to null to prevent dangling references.

## Benefits
*   **Zero Reference Passing:** Systems that need to create Vulkan objects (buffers, images, pipelines) can just call `lisa::graphics::Context::GetDevice()` and use it.
*   **Strict Lifetime Control:** The `Init()` and `Shutdown()` functions guarantee that Vulkan objects are created and destroyed in the exact required order.
*   **Clean Interfaces:** Class constructors don't need to be polluted with `LogicalDevice&` parameters, resulting in a cleaner and more readable codebase.