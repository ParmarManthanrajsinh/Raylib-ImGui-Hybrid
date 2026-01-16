# Raylib + ImGui Hybrid Engine

A high-performance codebase template combining **Raylib** (rendering) and **Dear ImGui** (UI) into a robust "Game Engine Editor" architecture. 

It features a **Multi-Threaded Render Loop** that completely solves the notorious "Window Freeze" issue on Windows, ensuring your game keeps running smoothly even while dragging, resizing, or clicking the title bar.

## 🚀 Key Features

*   **Hybrid Architecture**: Raylib handles the game scene (offscreen), while ImGui handles the windowing and tools.
*   **Threaded Rendering**: A dedicated Render Thread decoupled from the OS Event Loop.
    *   **Zero Freeze**: Dragging/Holding the window does *not* pause the game loop.
    *   **Zero Stutter**: High-performance, uncapped rendering independent of Windows message pumping.
*   **Unreal Engine Standards**: The codebase adheres to professional conventions:
    *   `F` prefix for classes (`FApplication`).
    *   `b` prefix for booleans.
    *   PascalCase for members.
    *   Allman-style formatting.
*   **Editor-Ready**: Your game runs inside a dockable, resizable viewport—just like Unity or Unreal.
*   **Self-Contained**: Statically linked dependencies (Raylib + ImGui).

## 🛠 Prerequisites

*   **CMake** (3.21+)
*   **C++ Compiler** (Visual Studio 2022 recommended)
*   **Git**

## 📦 Getting Started

### Windows (Visual Studio)

1.  **Configure**:
    ```powershell
    cmake -S . -B out/build
    ```
2.  **Build**:
    ```powershell
    cmake --build out/build --config Release
    ```
3.  **Run**:
    ```powershell
    .\out\build\Release\raylib_imgui_hybrid.exe
    ```

### Linux / macOS

1.  **Build & Run**:
    ```bash
    cmake -S . -B build
    cmake --build build
    ./build/raylib_imgui_hybrid
    ```

## 🧠 Architecture Overview

### 1. The Separation
Unlike standard Raylib apps, we do not run logic in the main `while(!WindowShouldClose)` loop.
*   **Main Thread**: Dedicated to OS interaction (`glfwWaitEvents`). It sleeps constantly, waking only for Input/Window messages. This makes the UI feel incredibly responsive.
*   **Render Thread**: Runs `FApplication::RenderLoop`. This thread owns the OpenGL context and runs as fast as possible, handling all Game Logic and Rendering.

### 2. The Abstraction
*   **Core**: The engine logic (`FApplication`, `EntryPoint`) is separated from user code.
*   **Sandbox**: The user app (`FSandboxApp`) inherits from `FApplication` and simply overrides `OnStart`, `OnUpdate`, and `OnUIRender`.

### 3. The Coding Standard
We follow a strict set of rules inspired by Unreal Engine 5:
*   **Classes**: `class FApplication`
*   **Variables**: `int Width;`, `bool bIsRunning;`
*   **Formatting**:
    ```cpp
    void Function() 
    {
        if (bCondition)
        {
            DoSomething();
        }
    }
    ```

## 📂 Project Structure

*   `src/Core/`: Engine internals (App loop, Threading, EntryPoint).
*   `src/main.cpp`: The user application (Sandbox).
*   `external/`: Raylib and ImGui sources.
*   `CMakeLists.txt`: Build configuration.

---
*License: MIT*
