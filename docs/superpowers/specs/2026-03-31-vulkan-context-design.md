# Vulkan Context Design Spec

## Overview
This specification details a clean architecture for managing global Vulkan state within the `lisa-vulkan` engine. The goal is to provide easy access to the core Vulkan context (`Instance`, `PhysicalDevice`, `LogicalDevice`, `Queue`s) without passing a multitude of references or pointers around the codebase.

## Design Approach: Static Namespace with Custom Wrappers & `vk::raii`
The engine will use a static namespace pattern combined with custom engine-level wrapper classes around Vulkan-Hpp's `vk::raii` module. All main Vulkan types will have corresponding wrappers in the engine (e.g., `lisa::graphics::Instance`, `lisa::graphics::LogicalDevice`, `lisa::graphics::PhysicalDevice`), some of which are already written.

### 1. Architecture
*   **Location:** A new module `src/lisa/graphics/Context.h` and `src/lisa/graphics/Context.cpp`.
*   **Namespace:** `lisa::graphics::Context`
*   **Visibility:** The header will expose simple accessor functions (e.g., `GetDevice()`). The `.cpp` file will hold the static instances of the engine's custom wrapper classes (wrapped in `std::unique_ptr` to explicitly manage initialization/destruction order).

### 2. State Management (Custom Wrappers)
The `Context.cpp` file will manage the lifetime of the core wrapper objects. By using wrappers, the engine abstracts away raw `vk::raii` initialization details and embeds custom logic (like device scoring, layer validation, etc.):
*   `std::unique_ptr<vk::raii::Context>` (Base Vulkan context, usually raw as it has no custom logic)
*   `std::unique_ptr<lisa::graphics::Instance>` (Wrapper holding `vk::raii::Instance`)
*   `std::unique_ptr<lisa::graphics::PhysicalDevice>` (Wrapper holding `vk::raii::PhysicalDevice`)
*   `std::unique_ptr<lisa::graphics::LogicalDevice>` (Wrapper holding `vk::raii::Device`)

### 3. Accessors
The header will provide globally accessible functions returning references to the custom wrapper objects:
*   `lisa::graphics::Instance& GetInstance();`
*   `lisa::graphics::PhysicalDevice& GetPhysicalDevice();`
*   `lisa::graphics::LogicalDevice& GetDevice();`

*(Note: Accessing the raw `vk::raii` handles or standard `vk::` handles should be done through getter methods inside the wrapper classes themselves, e.g., `GetDevice().vk_device()`)*

### 4. Initialization and Cleanup
Even with `vk::raii` powering the wrappers, global static variables have an undefined destruction order across different translation units. To guarantee the `Device` is destroyed before the `Instance`, we use explicit initialization and shutdown functions:
*   `void Init();` - Called once at application startup. This creates the base `vk::raii::Context`, then creates the custom `lisa::graphics::Instance`, selects a `lisa::graphics::PhysicalDevice`, and creates the `lisa::graphics::LogicalDevice`.
*   `void Shutdown();` - Called once at application shutdown. This calls `.reset()` on the `unique_ptr`s in the reverse order of creation (Device -> PhysicalDevice -> Instance -> Context).

## Benefits
*   **Encapsulation:** Wrappers provide an abstraction layer over Vulkan initialization, separating business logic from raw API calls.
*   **Zero Reference Passing:** Systems that need to interact with Vulkan can just call `lisa::graphics::Context::GetDevice()` and use it.
*   **Safe RAII Semantics:** Memory safety is maintained using Vulkan-Hpp's RAII wrappers inside the custom classes, while explicit initialization guarantees correct dependency ordering.
*   **Clean Interfaces:** Class constructors don't need to be polluted with `LogicalDevice&` parameters, resulting in a cleaner and more readable codebase.