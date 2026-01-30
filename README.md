# Raylib + ImGui Hybrid Engine

A high-performance codebase template providing a robust **Game Engine Architecture** by combining **Raylib** for rendering and **Dear ImGui** for tooling.

![Badge](https://img.shields.io/badge/Language-C++23-blue)
![Badge](https://img.shields.io/badge/Platform-Windows%20|%20Linux%20|%20macOS-lightgrey)
![Badge](https://img.shields.io/badge/License-MIT-green)

---

## 📖 Overview

This project is not just a wrapper; it is a **Full Application Framework** designed to solve common challenges in Game Development tooling:
*   **Multi-Threaded Architecture**: Solves the "Window Freeze" issue on Windows. Game logic runs on a dedicated thread, ensuring 100% responsiveness even when blocking the OS message loop.
*   **Event-Driven**: A propagation system similar to professional engines (Unreal/Unity), allowing UI overlay layers to block events from reaching the game world.
*   **Modern C++ Standards**: Built with **C++23** features, enforcing strict type safety, RAII resource management, and clean code practices.
*   **Editor-Ready**: Includes a pre-configured Docking layout, Viewport rendering, and Console logging.

## 🚀 Features

### Core Architecture
*   **Hybrid Rendering**: Raylib renders to an offscreen framebuffer, which is then drawn as an ImGui Image. This allows your game to exist as a "Window" within the Editor toolset.
*   **Layer Stack**: Modularize your logic. Push an `Overlay` for UI (always on top) or a `Layer` for game worlds.
*   **Event System**: 
    *   Categories: `Application`, `Input`, `Keyboard`, `Mouse`.
    *   Dispatch: `FEventDispatcher` uses C++ Concepts to ensure type safety.
    *   Blocking: If a Layer handles an event (`bHandled = true`), lower layers never see it.

### Developer Experience
*   **Unreal Engine Style**: Clean, professional naming conventions (`FClass`, `bBool`, PascalCase).
*   **Hot-Reloadable UI**: ImGui docking layout prevents window clutter.
*   **RAII Wrappers**: `FScopedTexture`, `FScopedModel`, and `FScopedRenderTexture` ensure you never leak VRAM.
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
    ```powershell
    cmake -S . -B out/build
    cmake --build out/build --config Release
    ```

3.  **Run**
    ```powershell
    .\out\build\Release\raylib_imgui_hybrid.exe
    ```

---

## 🧠 Deep Dive: Systems

### 1. The Threading Model
Unlike a standard game loop `while(!WindowShouldClose)`, we decouple the **OS Loop** from the **Render Loop**.
*   **Main Thread**: Handles `glfwWaitEvents`. It sleeps until the OS sends a signal (Mouse, Key, Resize). This keeps the app roughly 0% CPU usage when idle and incredibly responsive.
*   **Render Thread**: Runs `FApplication::RenderLoop`. This acts as the "Game Thread". It owns the OpenGL Context and pumps frames as fast as `Interval` allows.

### 2. The Layer Stack
Everything in the engine is a `FLayer`. 
```cpp
class GameLayer : public Core::FLayer {
    void OnUpdate(float DeltaTime) override {
        // Run Physics, AI, Game Logic
    }
    void OnUIRender() override {
        // Draw ImGui Windows
    }
    void OnEvent(Core::FEvent& Event) override {
        // Handle Input
    }
};
```
Pushing layers is done in your Application constructor:
```cpp
PushLayer(new GameLayer());
PushOverlay(new ConsoleLayer());
```

### 3. Resource Management (RAII)
We strictly avoid manual `Load/Unload` calls to prevent leaks.
*   **Bad**: `Texture2D tex = LoadTexture(...)` (Requires manual `UnloadTexture`)
*   **Good**: `Core::FScopedTexture tex("assets/hero.png");` (Automatic cleanup)

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
