#include "Application.h"
#include "Core/Logging/Log.h"
#include "Core/Input/Input.h"
#include "Core/Renderer/AssetManager.h"
#include <raylib-cpp.hpp>

#include "GLFW/glfw3.h"

extern "C" 
{
    #include "rlgl.h"
}

namespace Core 
{
    FApplication* FApplication::s_Instance = nullptr;

    FApplication::FApplication(const FApplicationConfig& InConfig)
        : Config(InConfig),
          Name(InConfig.Name), 
          Width(InConfig.Width), 
          Height(InConfig.Height),
          WindowHandle(nullptr),
          bIsRunning(false)
    {
        CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;
    }

    FApplication::~FApplication()
    {
    }

    void FApplication::PushLayer(FLayer* InLayer) { LayerStack.PushLayer(InLayer); }
    void FApplication::PushOverlay(FLayer* InOverlay) { LayerStack.PushOverlay(InOverlay); }

    void FApplication::OnEvent(FEvent& InEvent)
    {
        FEventDispatcher Dispatcher(InEvent);
        Dispatcher.Dispatch<FWindowCloseEvent>(CORE_BIND_EVENT_FN(FApplication::OnWindowClose));
        Dispatcher.Dispatch<FWindowResizeEvent>(CORE_BIND_EVENT_FN(FApplication::OnWindowResize));

        for (auto It = LayerStack.rbegin(); It != LayerStack.rend(); ++It)
        {
            if (InEvent.bHandled) break;
            (*It)->OnEvent(InEvent);
        }
    }

    bool FApplication::OnWindowClose(FWindowCloseEvent& e)
    {
        bIsRunning = false;
        return true;
    }

    bool FApplication::OnWindowResize(FWindowResizeEvent& e)
    {
        if (e.GetWidth() == 0 || e.GetHeight() == 0) return false;
        SetSize(e.GetWidth(), e.GetHeight());
        return false;
    }

    void FApplication::Run()
    {
        // 1. Let Raylib natively initialize the Window, OpenGL context, and internal shader/texture states!
        InitWindow(Width, Height, Name.c_str());
        SetTargetFPS(60);

        // Raylib abstracts the exact window pointer. We extract the true GLFW window handle securely from the current context.
        WindowHandle = glfwGetCurrentContext();
        bIsRunning = true;

        // --- Execute User Hooks ---
        OnMainThreadInit();
        OnRenderThreadInit();

        OnStart();

        while (!WindowShouldClose() && bIsRunning)
        {
            float DeltaSeconds = GetFrameTime();

            OnPreUpdate();

            for (FLayer* Layer : LayerStack)
                Layer->OnUpdate(DeltaSeconds);

            OnUpdate(DeltaSeconds);

            // 2. Wrap all rendering in native Raylib drawing macros
            BeginDrawing();
            ClearBackground(raylib::Color{30, 30, 30, 255});

            for (FLayer* Layer : LayerStack)
                Layer->OnUIRender();

            OnUIRender();

            OnPostRender();

            EndDrawing();
        }

        OnShutdown();

        // --- Execute User Hooks Clean ---
        OnRenderThreadShutdown();
        OnMainThreadShutdown();

        // Safely wipe VRAM models from memory BEFORE OpenGL shuts down!
        Core::AssetManager::Clear();

        CloseWindow();
    }
}
