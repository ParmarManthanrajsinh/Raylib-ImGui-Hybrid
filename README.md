# Raylib + ImGui Hybrid Engine

A high-performance codebase template providing a robust **AAA Modular Game Engine Architecture** by combining **Raylib** for rendering and **Dear ImGui** for advanced editor tooling.

![Badge](https://img.shields.io/badge/Language-C++23-blue)
![Badge](https://img.shields.io/badge/Platform-Windows%20|%20Linux%20|%20macOS-lightgrey)
![Badge](https://img.shields.io/badge/License-MIT-green)

---

## 📖 Overview

This project is a highly-optimized **Full Application Framework**. It mirrors the architecture of professional-grade software like Godot and Unreal Engine, physically decoupling your tools from your core shipping game logic.

*   **Modular 4-Pillar Architecture**: Splits development into `Engine.lib`, `Game.lib`, `Editor.exe`, and a hyper-lightweight `GameLauncher.exe`.
*   **VRAM Safe AssetManager**: Global registry pattern protecting GPU meshes and textures from accidental STL reallocation destructions.
*   **Event-Driven**: A propagation system allowing UI overlay layers to cleanly block OS events from hitting the game world securely.
*   **Modern C++ Standards**: Built with **C++23** features, enforcing strict type safety, smart pointers, RAII management, and generic lambdas.
*   **LTO Accelerated**: Includes a highly optimized `CMakeSettings.json` injecting `AVX2`, Fast-Math, and Link-Time-Optimization globally for the Release binary.

## 🚀 Features

### Developer Experience
*   **Unreal Engine Style Architecture**: Clean, professional naming conventions (`FClass`, `bBool`, PascalCase) mixed with deep modern C++ standard integration.
*   **Object-Oriented API**: Fully integrated with **raylib-cpp**, providing overloaded math operators and inline method calls inherently over legacy C-functions.
*   **Editor-Ready**: The included `Editor.exe` uses a pre-configured Docking layout, Viewport rendering extraction, and Inspector mapping. Code your game once, and visualize it dynamically in the Editor dynamically.
*   **Logging**: Thread-safe colored logging via `FLog`.

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

### 1. The Asset Manager & VRAM Protection
We strictly avoid manual `Load/Unload` lifecycles on the Game Layer. Through the `Core::AssetManager`, physical model loading requests are cached centrally in a `std::unordered_map` and distributed out as `std::shared_ptr<raylib::Model>`.
*   **The Benefit**: When copying arrays of generated Entities via `std::vector`, underlying GPU materials are securely preserved by reference. 
*   **The Safety**: The Application framework manually invokes `AssetManager::Clear()` to deliberately sweep all GPU data accurately moments before OpenGL unlinks natively.

### 2. The Layer Stack
Everything in the engine executes within a `FLayer`. 
```cpp
class GameLayer : public Core::FLayer {
    void OnUpdate(float DeltaTime) override {
        // Run Physics, Transforms, Game Logic via ActiveScene
    }
    void OnUIRender() override {
        // Request visual models accurately natively onto Raylib's BeginDrawing context
    }
};
```
Pushing layers into the host Application stack configures them automatically:
```cpp
// Inside GameLauncher.exe
PushLayer(new FGameLayer());
```

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

## 🤝 Contribution

Contributions are welcome! Please ensure any Pull Requests adhere to the **Coding Standard** listed above. 
1.  Fork the Project
2.  Create your Feature Branch
3.  Commit your Changes
4.  Open a Pull Request

---

*Built with ❤️ using Raylib & Dear ImGui*
