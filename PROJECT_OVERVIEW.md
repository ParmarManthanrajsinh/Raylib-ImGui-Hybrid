# Raylib-ImGui Hybrid Engine

## Project Overview
This project is a high-performance, modern C++ (C++23) game engine framework that hybridizes **Raylib** for 3D/2D rendering, **EnTT** for data-oriented Entity-Component-System architecture, and **ImGui** for sophisticated tooling and user interfaces. It is designed with a **Modular 4-Pillar Architecture** heavily inspired by professional AAA game engines (like Unreal Engine and Hazel), promoting a pristine separation of concerns between tools, platform logic, and gameplay.

The core philosophy is **"Code as Data, UI as Tooling"**. The engine manages the heavy lifting—hardware windowing, events, VRAM asset caching, rendering contexts—allowing developers to focus entirely on pure gameplay logic and robust editor environments.

## Core Architecture (The 4 Pillars)

### 1. `Engine.lib` (The Framework Backend)
The foundation of the software. It has zero knowledge of your specific game or heavy editor toolchains (no ImGui logic).
- **Initialization**: Single-threaded native Raylib window lifecycle management.
- **Game Loop**: A decoupled `FApplication` loop that runs per-frame in this order:
  1. **OnUpdate(DeltaTime)**: Logic tick — physics, game rules, ECS systems.
  2. **BeginDrawing()**: Raylib begins the GPU frame.
  3. **OnRender()**: 3D scene drawing — models, meshes, shaders.
  4. **OnUIRender()**: 2D/ImGui overlay drawing.
  5. **EndDrawing()**: Raylib submits the frame.
- **Resource Limits**: RAII-compliant cleanup of all VRAM resources via `AssetManager::Clear()` before `CloseWindow()`.
- **Layer Ownership**: The `LayerStack` owns all layers via `std::unique_ptr<FLayer>`, guaranteeing deterministic cleanup with no memory leaks.

### 2. `Game.lib` (Your Game Logic)
Developers spend 90% of their time here. Compiles as a static library containing all game rules, levels, and logic.
- Inherits from `Core::FLayer` (`FGameLayer`) which holds an `ActiveScene`.
- Entities are created via the ECS factory: `ActiveScene->CreateEntity("Name")`.
- Components are attached freely: `entity.AddComponent<ModelComponent>(...)`.
- The `RenderScene()` method uses `entt::view` to iterate only entities with the required components for rendering.

### 3. `Editor.exe` (The Development Tool)
- Embeds both `Engine.lib` and `Game.lib`. 
- Heavily utilizes the `ImGui` docking branch. 
- Intercepts the `GameLayer`'s rendering pipeline via `OnRender()` and redirects geometry into an **ImGui Viewport Panel** using a `RenderTexture2D` framebuffer.
- Provides a **Scene Hierarchy** panel (iterates `TagComponent` via the registry) and a **Property Inspector** panel (conditionally displays component editors using `HasComponent<T>()`).

### 4. `GameLauncher.exe` (The Shipped Game)
- A highly optimized, microscopic executable that you ship to players.
- Completely strips out ImGui and visual editor overhead.
- Pushes `FGameLayer` to the native window and renders via `OnRender()`.

## Key Engine Subsystems

### System: Entity-Component-System (EnTT)
The engine uses [EnTT v3.16.0](https://github.com/skypjack/entt) as its ECS backend. This replaces the old `std::vector<FEntity>` pattern with a proper data-oriented architecture.

**Core types:**
- `entt::registry` — Central database for all entities and components (owned by `FScene`).
- `entt::entity` — A lightweight 32-bit handle identifying an entity.
- `FEntity` — A thin C++ wrapper providing `AddComponent<T>()`, `GetComponent<T>()`, `HasComponent<T>()`, and `RemoveComponent<T>()`.

**Built-in components:**
| Component | Purpose | Key Fields |
| :--- | :--- | :--- |
| `TagComponent` | Human-readable name | `std::string Name` |
| `TransformComponent` | 3D position, rotation, scale | `Position`, `RotationAxis`, `Angle`, `Scale` |
| `ModelComponent` | GPU mesh reference | `std::shared_ptr<raylib::Model>` |
| `MeshRendererComponent` | Visual overrides | `BaseColor`, `bDrawWireframe` |

**Usage:**
```cpp
Core::FEntity Cube = ActiveScene->CreateEntity("Demo Cube");
Cube.AddComponent<Core::ModelComponent>(AssetManager::RegisterModel("Cube", raylib::Model(GenMeshCube(1, 1, 1))));
Cube.AddComponent<Core::MeshRendererComponent>(raylib::Color(230, 41, 55, 255));
```

### System: Asset Manager
To prevent memory leaks and VRAM corruption, the engine utilizes a central `Core::AssetManager`. Because ECS component pools may relocate memory internally, if components directly stored raw `raylib::Model` instances, their GPU resources could be silently freed by RAII destructors during pool resizing. The Asset Manager mitigates this by caching models in a `std::unordered_map` and distributing only `std::shared_ptr<raylib::Model>` references to components.

### System: The Layer Stack
Logic is compartmentalized into **Layers** (`Core::FLayer`). The application maintains a `LayerStack` that owns all layers via `std::unique_ptr`.

**Layer lifecycle hooks:**
| Hook | Timing | Purpose |
| :--- | :--- | :--- |
| `OnAttach()` | Once, when pushed | Initialization, entity spawning |
| `OnDetach()` | Once, when popped/destroyed | Cleanup |
| `OnUpdate(DeltaTime)` | Every frame, before drawing | Physics, game logic, ECS systems |
| `OnRender()` | Every frame, inside `BeginDrawing`/`EndDrawing` | 3D scene rendering |
| `OnUIRender()` | Every frame, after `OnRender()` | ImGui / 2D overlay drawing |
| `OnEvent(Event)` | On OS events | Input handling; layers can flag `bHandled` to consume events |

### System: Event Dispatcher
Events use a type-safe dispatcher with C++20 `<concepts>`:
```cpp
template<typename T, typename F>
requires std::derived_from<T, FEvent>
bool Dispatch(const F& Func);
```
This guarantees at compile time that you can only dispatch to valid event types.

## Key Build Technologies
- **CMake**: The build system orchestrates `add_subdirectory` hierarchies for all four module pillars.
- **CMakeSettings.json**: The `Release` profile enables extreme MSVC flags including **AVX2** SIMD, fast-math (`/fp:fast`), and full **Link-Time-Optimization (LTCG/GL)**.
- **C++23**: `std::format_string` for compile-time validated logging, `<concepts>` for type-safe event dispatch, generic lambdas for event binding.
- **raylib-cpp**: Object-oriented C++ wrapper over the Raylib C API.
- **EnTT 3.16.0**: Header-only ECS library, included via `external/entt-3.16.0/single_include`.

## Current State
The architecture provides a fully functional, scalable foundation for building modern 3D games and complex visual editing tools. The ECS is operational with component creation, querying, and iteration. The render pipeline cleanly separates logic updates from 3D rendering from UI drawing. Both the Editor and GameLauncher targets compile and run correctly.
