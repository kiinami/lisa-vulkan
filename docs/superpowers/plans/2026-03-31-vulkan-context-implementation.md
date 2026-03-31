# Vulkan Context Implementation Plan

> **For agentic workers:** REQUIRED: Use superpowers:subagent-driven-development (if subagents available) or superpowers:executing-plans to implement this plan. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Implement a static namespace architecture for managing global Vulkan state, utilizing custom `lisa::graphics` wrappers around `vk::raii` objects to eliminate reference passing.

**Architecture:** We will create a `lisa::graphics::Context` namespace holding `unique_ptr`s to `vk::raii::Context`, `lisa::graphics::Instance`, `lisa::graphics::PhysicalDevice`, and `lisa::graphics::LogicalDevice`. Accessors will be exposed in `Context.h`, and state initialized via `Init()` and destroyed via `Shutdown()`.

**Tech Stack:** C++20, Vulkan-Hpp (`vk::raii`), Meson

---

## Chunk 1: Define `Context.h` and Accessors

### Task 1: Create `Context.h` and declare namespace

**Files:**
- Create/Modify: `src/lisa/graphics/Context.h`
- Modify: `src/lisa/graphics/Context.cpp`

- [ ] **Step 1: Write header definitions in `Context.h`**

```cpp
#pragma once
#include <memory>
#include <vulkan/vulkan_raii.hpp>
#include "device/Instance.h"
#include "device/PhysicalDevice.h"
#include "device/LogicalDevice.h"

namespace lisa::graphics::Context {
    void Init();
    void Shutdown();

    vk::raii::Context& GetVulkanContext();
    Instance& GetInstance();
    PhysicalDevice& GetPhysicalDevice();
    LogicalDevice& GetDevice();
}
```

- [ ] **Step 2: Add initial stubs to `Context.cpp`**

```cpp
#include "Context.h"
#include <stdexcept>

namespace lisa::graphics::Context {

    static std::unique_ptr<vk::raii::Context> s_Context;
    static std::unique_ptr<Instance> s_Instance;
    static std::unique_ptr<PhysicalDevice> s_PhysicalDevice;
    static std::unique_ptr<LogicalDevice> s_LogicalDevice;

    void Init() {
        s_Context = std::make_unique<vk::raii::Context>();
        // Instance, PhysicalDevice, and LogicalDevice creation will go here in next chunks
    }

    void Shutdown() {
        s_LogicalDevice.reset();
        s_PhysicalDevice.reset();
        s_Instance.reset();
        s_Context.reset();
    }

    vk::raii::Context& GetVulkanContext() { return *s_Context; }
    Instance& GetInstance() { return *s_Instance; }
    PhysicalDevice& GetPhysicalDevice() { return *s_PhysicalDevice; }
    LogicalDevice& GetDevice() { return *s_LogicalDevice; }
}
```

- [ ] **Step 3: Ensure building works**

Run: `meson compile -C buildDir`
Expected: Compile succeeds.

- [ ] **Step 4: Commit Chunk 1**

```bash
git add src/lisa/graphics/Context.h src/lisa/graphics/Context.cpp
git commit -m "feat: add Context namespace and basic wrappers"
```

## Chunk 2: Update Application Entry Point

### Task 2: Initialize Context in Main

**Files:**
- Modify: `src/main.cpp`

- [ ] **Step 1: Update `main.cpp` to call `Init` and `Shutdown`**

```cpp
#include "lisa/graphics/Context.h"
#include <iostream>

int main() {
    try {
        lisa::graphics::Context::Init();
        std::cout << "Vulkan Context Initialized Successfully\n";
        
        // App logic...
        
        lisa::graphics::Context::Shutdown();
        std::cout << "Vulkan Context Shut Down Successfully\n";
    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
```

- [ ] **Step 2: Build and run the app**

Run: `meson compile -C buildDir && ./buildDir/lisa-vulkan`
Expected: Program prints success messages without crashing.

- [ ] **Step 3: Commit Chunk 2**

```bash
git add src/main.cpp
git commit -m "feat: init and shutdown vulkan context in main"
```