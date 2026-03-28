# Raylib-ImGui Hybrid Engine

## Project Overview
This project is a high-performance, modern C++ (C++23) game engine framework that hybridizes **Raylib** for 3D/2D rendering and **ImGui** for sophisticated tooling and user interfaces. It is designed with a **Modular 4-Pillar Architecture** heavily inspired by professional AAA game engines (like Unreal Engine and Hazel), promoting a pristine separation of concerns between tools, platform logic, and gameplay.

The core philosophy is **"Code as Data, UI as Tooling"**. The engine manages the heavy lifting—hardware windowing, events, VRAM asset caching, rendering contexts—allowing developers to focus entirely on pure gameplay logic and robust editor environments securely.

## Core Architecture (The 4 Pillars)

### 1. `Engine.lib` (The Framework Backend)
The foundation of the software. It has zero knowledge of your specific game or heavy editor toolchains (no ImGui logic).
- **Initialization**: Single-Threaded native Raylib window lifecycle management.
- **Game Loop**: A decoupled `FApplication` loop that handles:
  1.  **Poll Events**: WindowResize, KeyPressed, MouseMoved are dispatched.
  2.  **Layer Updates**: Logic ($T_{delta}$) is ticked natively.
  3.  **UI Rendering**: Virtual hook `OnUIRender` for transparent GUI layers.
  4.  **Hardware Render**: `BeginDrawing`/`EndDrawing` macros perfectly bound to the loop.
- **Resource Limits**: RAII-compliant cleanup of all VRAM resources on explicit `CloseWindow()`.

### 2. `Game.lib` (Your Game Logic)
Developers spend 90% of their time here. Compiles as a static library containing all game rules, levels, and logic.
- Inherits from `Core::FLayer` (`FGameLayer`) which holds an `ActiveScene`.
- You spawn objects (Entities) by grabbing `raylib::Models` from the `AssetManager` securely.

### 3. `Editor.exe` (The Development Tool)
- Embeds both `Engine.lib` and `Game.lib`. 
- Heavily utilizes the `ImGui` docking branch. 
- It intercepts the `GameLayer`'s rendering pipeline and actively redirects its geometry into an **ImGui Viewport Panel**! Provides Scene Hierarchies and realtime Property Inspectors.

### 4. `GameLauncher.exe` (The Shipped Game)
- A highly optimized, microscopic executable that you ship to players.
- Completely strips out ImGui and visual editor overhead.
- Simply pushes your identical `Game.lib` directly to the native OS window natively.

## Key Engine Subsystems

### System: Asset Manager
To prevent memory leaks and VRAM dropping, the engine utilizes a central `Core::AssetManager`. 
Because `std::vector` resizes operations dynamically copy memory blocks, if Entities directly stored raw `raylib::Model` classes, their underlying GPUs textures would silently self-destruct during the vector relocation via their RAII destructors. The Asset Manager mitigates this exactly by caching raw memory models safely in a `std::unordered_map` and only handing out lightweight `std::shared_ptr<raylib::Model>` references to dynamic level components.

### System: The Layer Stack
Logic is compartmentalized into **Layers** (`Core::FLayer`). The application maintains a `LayerStack`.
- **OnAttach()**: Called when the layer is added (Initialization/Spawning).
- **OnUpdate(DeltaTime)**: Frame-by-frame entity manipulation logic.
- **OnUIRender()**: Native GPU drawing hook executing safely inside standard raylib blocks.
- **OnEvent(Event)**: Event interception (mouse dynamics). Layers can flag `bHandled` to prevent events bubbling deep into the game world if they click on UI.

## Key Build Technologies
- **CMake**: The build system dynamically orchestrates `add_subdirectory` hierarchies for all four module pillars simultaneously.
- **CMakeSettings.json**: The `Release` profile engages extreme MSVC flags including **AVX2** SIMD arrays, Native Fast-Math CPU instructions, and full Interprocedural **Link-Time-Optimization (LTCG / GL)**.
- **C++23**: Utilizing modern features like generic lambdas, `std::shared_ptr` concepts, and `<concepts>` logic masking for extreme type safety.
- **raylib-cpp**: Backend for Object-Oriented wrapper functionality over the standard Raylib C-structs.

## Current State
This architecture serves as a perfectly scalable cornerstone for building modern 3D games and complex proprietary visual editing tools synchronously.
