#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <raylib-cpp.hpp>

#include "Core/Events/Event.h"
#include "Core/Events/ApplicationEvent.h"
#include "Core/Layers/LayerStack.h"
#include "Core/Application/ApplicationConfig.h"

struct GLFWwindow;

namespace Core 
{
    class FApplication 
    {
    public:
        FApplication(const FApplicationConfig& InConfig = FApplicationConfig());
        virtual ~FApplication();
        static FApplication& Get() { return *s_Instance; }

        void Run();
        void OnEvent(FEvent& InEvent);

        void PushLayer(FLayer* InLayer);
        void PushOverlay(FLayer* InLayer);

        // Virtual Hooks for Injected Systems (Like ImGui/Editor tooling)
        virtual void OnMainThreadInit() {}
        virtual void OnMainThreadShutdown() {}
        virtual void OnRenderThreadInit() {}
        virtual void OnRenderThreadShutdown() {}
        virtual void OnPreUpdate() {}
        virtual void OnPostRender() {}

        // User Virtuals
        virtual void OnStart() {}
        virtual void OnUpdate(float DeltaTime) {}
        virtual void OnUIRender() {}
        virtual void OnShutdown() {}
        
        [[nodiscard]] GLFWwindow* GetWindow() const { return WindowHandle; }
        
        // Sync data
        [[nodiscard]] int GetWidth() const { return Width; }
        [[nodiscard]] int GetHeight() const { return Height; }
        void SetSize(int NewWidth, int NewHeight) { Width = NewWidth; Height = NewHeight; }

    private:
        bool OnWindowClose(FWindowCloseEvent& e);
        bool OnWindowResize(FWindowResizeEvent& e);

    private:
        std::string Name;
        FApplicationConfig Config;
        
        std::atomic<int> Width;
        std::atomic<int> Height;
        GLFWwindow* WindowHandle;
        
        FLayerStack LayerStack;

        bool bIsRunning;
    
    private:
        static FApplication* s_Instance;
    };
}
