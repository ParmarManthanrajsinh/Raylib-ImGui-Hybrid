# Raylib + ImGui Hybrid Engine

A high-performance codebase template providing a robust **AAA Modular Game Engine Architecture** by combining **Raylib** for rendering, **EnTT** for Entity-Component-System management, and **Dear ImGui** for advanced editor tooling.

![Badge](https://img.shields.io/badge/Language-C++23-blue)
![Badge](https://img.shields.io/badge/Platform-Windows%20|%20Linux%20|%20macOS-lightgrey)
![Badge](https://img.shields.io/badge/License-MIT-green)

---

## 📖 Overview

This project is a highly-optimized **Full Application Framework**. It mirrors the architecture of professional-grade software like Godot and Unreal Engine, physically decoupling your tools from your core shipping game logic.

*   **Modular 4-Pillar Architecture**: Splits development into `Engine.lib`, `Game.lib`, `Editor.exe`, and a hyper-lightweight `GameLauncher.exe`.
*   **EnTT Entity-Component-System**: Industry-standard data-oriented entity architecture. Entities are lightweight 32-bit handles; components are pure data structs stored in contiguous memory pools.
*   **VRAM Safe AssetManager**: Global registry pattern protecting GPU meshes and textures from accidental STL reallocation destructions.
*   **Event-Driven**: A propagation system allowing UI overlay layers to cleanly block OS events from hitting the game world securely.
*   **Modern C++ Standards**: Built with **C++23** features, enforcing strict type safety, smart pointers, RAII management, and generic lambdas.
*   **LTO Accelerated**: Includes a highly optimized `CMakeSettings.json` injecting `AVX2`, Fast-Math, and Link-Time-Optimization globally for the Release binary.

## 🚀 Features

### Developer Experience
*   **Unreal Engine Style Architecture**: Clean, professional naming conventions (`FClass`, `bBool`, PascalCase) mixed with deep modern C++ standard integration.
*   **Object-Oriented API**: Fully integrated with **raylib-cpp**, providing overloaded math operators and inline method calls inherently over legacy C-functions.
*   **True ECS**: Entities are composed of independent components (`TagComponent`, `TransformComponent`, `ModelComponent`, `MeshRendererComponent`) that can be freely mixed and matched.
*   **Editor-Ready**: The included `Editor.exe` uses a pre-configured Docking layout, Viewport rendering extraction, and Inspector mapping. Code your game once, and visualize it dynamically in the Editor.
*   **Logging**: Compile-time validated logging via `FLog` using `std::format_string`.

---

## 📦 Getting Started

### Prerequisites
*   **CMake** (3.21+)
*   **C++ Compiler** (Visual Studio 2022 recommended)
*   **Git**

### Installation

1.  **Clone the Repository**
    ```bash
    git clone https://github.com/YourRepo/Raylib-ImGui-Hybrid.git
    cd Raylib-ImGui-Hybrid
    ```

2.  **Build (Windows)**
    Using PowerShell, configure the modular CMake project and build the Release configuration natively:
    ```powershell
    cmake -S . -B out/build
    cmake --build out/build --config Release
    ```

3.  **Run the Editor**
    ```powershell
    .\out\build\Editor\Release\Editor.exe
    ```

4.  **Run the Shipped Game**
    Test the standalone minimal footprint of your underlying simulation:
    ```powershell
    .\out\build\GameLauncher\Release\GameLauncher.exe
    ```

---

## 🧠 Deep Dive: Systems

### 1. Entity-Component-System (EnTT)
The engine uses [EnTT](https://github.com/skypjack/entt) as its ECS backend. `FScene` owns an `entt::registry`; `FEntity` is a lightweight wrapper around a 32-bit `entt::entity` handle.

```cpp
// Create an entity with components
Core::FEntity Cube = ActiveScene->CreateEntity("Demo Cube");
Cube.AddComponent<Core::ModelComponent>(AssetManager::RegisterModel("Cube", raylib::Model(GenMeshCube(1, 1, 1))));
Cube.AddComponent<Core::MeshRendererComponent>(raylib::Color(230, 41, 55, 255));
Cube.GetComponent<Core::TransformComponent>().Position = {0, 2, 0};
```

Rendering iterates only entities that possess the required components:
```cpp
auto View = Registry.view<TransformComponent, ModelComponent>();
for (auto Entity : View) {
    auto& transform = View.get<TransformComponent>(Entity);
    auto& model     = View.get<ModelComponent>(Entity);
    model.Model->Draw(transform.Position, ...);
}
```

### 2. The Asset Manager & VRAM Protection
Through `Core::AssetManager`, physical model loading requests are cached centrally in a `std::unordered_map` and distributed out as `std::shared_ptr<raylib::Model>`.
*   **The Benefit**: ECS components reference models via shared pointers. GPU memory is never accidentally freed during entity creation or destruction.
*   **The Safety**: The Application framework invokes `AssetManager::Clear()` before OpenGL shuts down, ensuring orderly VRAM release.

### 3. The Layer Stack
Everything in the engine executes within a `FLayer`. The `LayerStack` owns all layers via `std::unique_ptr`, ensuring deterministic cleanup.

```cpp
class GameLayer : public Core::FLayer {
    void OnUpdate(float DeltaTime) override {
        // Tick physics, game logic, ECS systems
    }
    void OnRender() override {
        // 3D scene drawing (runs inside BeginDrawing/EndDrawing)
    }
    void OnUIRender() override {
        // ImGui or 2D overlay rendering
    }
};
```

The lifecycle per frame is: `OnUpdate()` → `BeginDrawing()` → `OnRender()` → `OnUIRender()` → `EndDrawing()`.

---

## 🎨 Coding Standard

We follow a strict set of rules inspired by **Unreal Engine 5** to maintain long-term maintainability.

| Feature | Convention | Example |
| :--- | :--- | :--- |
| **Classes** | Prefix with `F` | `class FApplication` |
| **Interfaces** | Prefix with `I` | `class IEntity` |
| **Enums** | Prefix with `E` | `enum class EEventType` |
| **Booleans** | Prefix with `b` | `bool bIsVisible` |
| **Members** | PascalCase (No `m_`) | `float Width;` |
| **Getters** | `[[nodiscard]]` | `[[nodiscard]] float GetWidth() const;` |

---

## 📁 Project Structure

```
Raylib-ImGui-Hybrid/
├── Engine/                    # Core framework (static library)
│   └── src/
│       ├── Core/
│       │   ├── Application/   # Window, main loop, layer management
│       │   ├── Base/          # Macros, platform detection, smart pointer aliases
│       │   ├── Events/        # Event types and dispatcher
│       │   ├── Input/         # Raylib input abstraction
│       │   ├── Layers/        # Layer and LayerStack (unique_ptr ownership)
│       │   ├── Logging/       # FLog (std::format based)
│       │   └── Renderer/      # AssetManager
│       └── Scene/             # FScene (entt::registry), FEntity, Components
├── Game/                      # Shared game logic (static library)
│   └── src/
│       └── GameLayer.cpp      # Entity spawning, scene setup, render loop
├── Editor/                    # ImGui editor (executable)
│   └── src/
│       ├── EditorApp.cpp      # ImGui lifecycle hooks
│       └── EditorLayer.cpp    # Hierarchy, Inspector, Viewport panels
├── GameLauncher/              # Shipped standalone game (executable)
│   └── src/
│       └── GameApp.cpp        # Minimal native window host
└── external/                  # Third-party dependencies
    ├── raylib/
    ├── raylib-cpp/
    ├── imgui/                 # Docking branch
    └── entt-3.16.0/           # Entity-Component-System
```

---

## 🤝 Contribution

Contributions are welcome! Please ensure any Pull Requests adhere to the **Coding Standard** listed above. 
1.  Fork the Project
2.  Create your Feature Branch
3.  Commit your Changes
4.  Open a Pull Request

---

*Built with ❤️ using Raylib, EnTT & Dear ImGui*
