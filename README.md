# Raylib + ImGui Hybrid Engine

A high-performance codebase template providing a robust **Game Engine Architecture** by combining **Raylib** for rendering and **Dear ImGui** for tooling.

![Badge](https://img.shields.io/badge/Language-C++23-blue)
![Badge](https://img.shields.io/badge/Platform-Windows%20|%20Linux%20|%20macOS%20|%20Web-lightgrey)
![Badge](https://img.shields.io/badge/License-MIT-green)

---

## 📖 Overview

This project is not just a wrapper; it is a **Full Application Framework** designed to solve common challenges in Game Development tooling:
*   **Multi-Threaded Architecture**: Solves the "Window Freeze" issue on Windows. Game logic runs on a dedicated thread, ensuring 100% responsiveness even when blocking the OS message loop.
*   **Event-Driven**: A propagation system similar to professional engines (Unreal/Unity), allowing UI overlay layers to block events from reaching the game world.
*   **Modern C++ Standards**: Built with **C++23** features, enforcing strict type safety, RAII resource management, and clean code practices.
*   **Editor-Ready**: Includes a pre-configured Docking layout, Viewport rendering, and Console logging.
*   **🌐 WebAssembly Support**: Run your game directly in the browser with a single command!

## 🚀 Features

### Core Architecture
*   **Hybrid Rendering**: Raylib renders to an offscreen framebuffer, which is then drawn as an ImGui Image. This allows your game to exist as a "Window" within the Editor toolset.
*   **Layer Stack**: Modularize your logic. Push an `Overlay` for UI (always on top) or a `Layer` for game worlds.
*   **Event System**: 
    *   Categories: `Application`, `Input`, `Keyboard`, `Mouse`.
    *   Dispatch: `FEventDispatcher` uses C++ Concepts to ensure type safety.
    *   Blocking: If a Layer handles an event (`bHandled = true`), lower layers never see it.

### Developer Experience
*   **Unreal Engine Style Architecture**: Clean, professional naming conventions (`FClass`, `bBool`, PascalCase) mixed with C++ standards.
*   **Object-Oriented API**: Fully integrated with **raylib-cpp**, providing overloaded math operators and inline method calls instead of verbose C-functions.
*   **RAII Resource Safety**: Seamless VRAM management. Models and Textures automatically unload when they go out of scope.
*   **Logging**: Thread-safe colored logging via `FLog`.

---

## 📦 Getting Started

### Prerequisites
*   **CMake** (3.21+)
*   **C++ Compiler** (Visual Studio 2022, GCC 11+, or Clang 14+)
*   **Git**
*   **Python** (3.6+ for WebAssembly build)

### Native Platform Build

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

### 🌐 WebAssembly Build (Run in Browser)

The engine fully supports WebAssembly, allowing you to run your application directly in a web browser.

#### One-Command Setup & Build

1.  **Install Emscripten SDK**
    ```bash
    python scripts/install_emsdk.py
    ```
    This script will:
    - Download and install Emscripten SDK
    - Set up the required tools
    - Provide instructions for environment setup

2.  **Activate the Environment**
    You MUST run the activation script for your platform in your current terminal before building:
    - **Windows (Command Prompt):** `C:\Users\%USERNAME%\emsdk-main\emsdk_env.bat`
    - **Windows (PowerShell):** `& "C:\Users\$env:USERNAME\emsdk-main\emsdk_env.ps1"`
    - **Linux/macOS:** `source ~/emsdk-main/emsdk_env.sh`

3.  **Build for Web**
    ```bash
    python scripts/build.py
    ```
    This will:
    - Configure CMake for Emscripten
    - Compile your code to WebAssembly
    - Generate HTML, JS, and WASM files
    - Start a local web server
    - Automatically open your browser

4.  **Manual Web Build** (Alternative)
    ```bash
    mkdir build-web
    cd build-web
    emcmake cmake .. -DPLATFORM=Web
    cmake --build .
    python -m http.server 8000
    ```
    Then open `http://localhost:8000/raylib_imgui_hybrid.html` in your browser.

#### WebAssembly Platform-Specific Notes

The engine automatically handles platform differences:

```cpp
#ifdef CORE_PLATFORM_WEB
    // WebAssembly-specific fixes
    ClearBackground({BgColor.r, BgColor.g, BgColor.b, 255});
    ImGui::GetIO().FontGlobalScale = 1.5f;  // Better UI scaling
#else
    // Native desktop behavior
    BgColor.ClearBackground();
#endif
```

**Known WebAssembly Behavior:**
- ✅ Full event system works (Keyboard, Mouse, Input)
- ✅ ImGui Docking and UI rendering
- ✅ RenderTexture for viewport
- ⚠️ Use raw Raylib C functions for 3D drawing (colors work properly)
- ⚠️ UI may require font scaling for better readability

---

## 🧠 Deep Dive: Systems

### 1. The Threading Model
Unlike a standard game loop `while(!WindowShouldClose)`, we decouple the **OS Loop** from the **Render Loop**.
*   **Main Thread**: Handles `glfwWaitEvents`. It sleeps until the OS sends a signal (Mouse, Key, Resize). This keeps the app roughly 0% CPU usage when idle and incredibly responsive.
*   **Render Thread**: Runs `FApplication::RenderLoop`. This acts as the "Game Thread". It owns the OpenGL Context and pumps frames as fast as `Interval` allows.

*Note: WebAssembly uses a single-threaded model with `emscripten_set_main_loop`.*

### 2. The Layer Stack
Everything in the engine is a `FLayer`. 
```cpp
class GameLayer : public Core::FLayer 
{
    void OnUpdate(float DeltaTime) override 
    {
        // Run Physics, AI, Game Logic
    }
    void OnUIRender() override 
    {
        // Draw ImGui Windows
    }
    void OnEvent(Core::FEvent& Event) override 
    {
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
We strictly avoid manual `Load/Unload` calls to prevent leaks. Through the **raylib-cpp** library, all backend C-structs are wrapped in classes holding their own lifecycles.
*   **Bad (C-Style)**: `Texture2D tex = LoadTexture(...)` (Requires manual `UnloadTexture`)
*   **Good (C++-Style)**: `raylib::Texture tex("assets/hero.png");` (Automatic cleanup)

*Warning*: If you initialize resources at the Application level class, wrap them in `std::optional<raylib::Model>` and `.reset()` them during `OnShutdown()` so they unload *before* the OpenGL context terminates on the render thread.

---

## 🎨 Coding Standard

We follow a strict set of rules inspired by **Unreal Engine 5** to maintain long-term maintainability.

| Feature | Convention | Example |
| :--- | :--- | :--- |
| **Classes** | Prefix with `F` | `class FApplication` |
| **Interfaces** | Prefix with `I` | `interface IEntity` |
| **Enums** | Prefix with `E` | `enum class EEventType` |
| **Booleans** | Prefix with `b` | `bool bIsVisible` |
| **Members** | PascalCase (No `m_`) | `float Width;` |
| **Getters** | `[[nodiscard]]` | `[[nodiscard]] float GetWidth() const;` |

---

## 🛠️ Project Structure

```
Raylib-ImGui-Hybrid/
├── scripts/
│   ├── install_emsdk.py      # Emscripten installer
│   └── build.py               # WebAssembly build script
├── external/
│   └── raylib/                # Raylib submodule
├── Core/                      # Engine framework
│   ├── Application/           # App lifecycle & layer stack
│   ├── Events/                # Event system
│   └── Log/                   # Logging system
├── Sandbox/                   # Example application
└── CMakeLists.txt             # Build configuration
```

---

## 🤝 Contribution

Contributions are welcome! Please ensure any Pull Requests adhere to the **Coding Standard** listed above. 
1.  Fork the Project
2.  Create your Feature Branch
3.  Commit your Changes
4.  Open a Pull Request

---

## 📝 License

Distributed under the MIT License. See `LICENSE` file for more information.

---

*Built with ❤️ using Raylib & Dear ImGui*