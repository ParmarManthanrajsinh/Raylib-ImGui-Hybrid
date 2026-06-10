# Raylib-ImGui Hybrid Engine

## Project Overview
This project is a high-performance, modern C++ game engine framework that hybridizes **Raylib** for 3D/2D rendering and **ImGui** for sophisticated tooling and user interfaces. It is designed with a **Layer-based Architecture** inspired by professional game engines (like Hazel or Unreal Engine patterns), promoting clean separation of concerns and modularity.

The core philosophy is **"Code as Data, UI as Tooling"**. The engine manages the heavy lifting—windowing, events, rendering contexts—allowing developers to focus on gameplay logic and editor tools.

## Architecture

### 1. Core Application Loop (`Core::FApplication`)
The heart of the engine is the `FApplication` class. It manages the lifecycle of the software:
- **Initialization**: Sets up the GLFW window, initializes Raylib, and configures the ImGui context (including Docking and Viewports).
- **Game Loop**: A decoupled loop that handles:
  1.  **Poll Events**: Inputs are captured and dispatched.
  2.  **Layer Updates**: Logic ($T_{delta}$) is processed.
  3.  **UI Rendering**: ImGui frames are generated.
  4.  **Render**: The final composite is presented to the screen.
- **Shutdown**: RAII-compliant cleanup of all resources.

### 2. Layer System (`Core::FLayer`)
Logic is compartmentalized into **Layers**. The application maintains a `LayerStack`.
- **OnAttach()**: Called when the layer is added (Initialization).
- **OnUpdate(DeltaTime)**: Frame-by-frame logic execution.
- **OnUIRender()**: dedicated pass for ImGui widgets.
- **OnEvent(Event)**: Event interception (mouse clicks, key presses).

*Example Usage:* A typical game might have a `GameLayer` for physics/rendering and a `UILayer` for the HUD.

### 3. Event System
The engine features a robust **Event Dispatcher** system. Events (WindowResize, KeyPressed, MouseMoved) are propagated through layers. Layers can "consume" events to prevent them from bubbling down (e.g., if ImGui captures the mouse, the game camera shouldn't move).

### 4. Renderer & Resource Management
To prevent memory leaks and manual resource management hell, the engine utilizes **raylib-cpp**, a comprehensive object-oriented wrapper for the Raylib API.
- All Raylib C-structs are replaced with their C++ counterparts (e.g., `raylib::Model`, `raylib::RenderTexture2D`).
- These classes function as **RAII (Resource Acquisition Is Initialization)** wrappers, automatically calling `Unload()` in their destructors.

*Note on Global/Layer lifetimes:* Because the OpenGL Context is destroyed early on the Main Thread during `Application::Run`, global objects are wrapped in `std::optional<T>` and explicitly reset (`.reset()`) during `OnShutdown()` which runs on the Render Thread. This ensures safe cleanup while the GPU context is still valid.

## Key Build Technologies
- **CMake**: The build system orchestrates the compilation of Raylib, ImGui (docking branch), and the engine source.
- **C++20**: Utilizing modern features like `std::format`, `concepts`, and specific casts for type safety.
- **Raylib**: Backend for OpenGL context and drawing.
- **Dear ImGui**: Backend for Editor UI, Inspector panels, and Docking.

## Code Map
- **`src/main.cpp`**: The user-side application definition (`FSandboxApp`). This is where you consume the engine API.
- **`src/Core/Application/`**: Application lifecycle and Entry Point.
- **`src/Core/Layers/`**: Base Layer classes.
- **`src/Core/Renderer/`**: Abstractions for Raylib resources.
- **`src/Core/Events/`**: Event definitions and Dispatcher logic.

## Getting Started
The current `main.cpp` implements a **Sandbox**:
- A 3D Viewport rendered to a Framebuffer.
- An ImGui Dockspace with a "Settings" panel.
- Real-time manipulation of a 3D Cube (Rotation, Color, Wireframe).
- Automatic viewport resizing handling.

This architecture serves as a solid foundation for building complex tools, 3D viewers, or games with integrated editors.
