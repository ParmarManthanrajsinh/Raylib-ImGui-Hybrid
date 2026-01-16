#pragma once

#include <string>
#include "imgui.h"

// Forward declaration to avoid including internal headers in the public API if possible, 
// strictly speaking user asked to use raylib methods, so including raylib.h here is good 
// so they don't have to include it manually in every file deriving from Application.
extern "C" {
#include "raylib.h"
}

struct GLFWwindow;

namespace Core {

    class Application {
    public:
        Application(const std::string& name = "App", int width = 1280, int height = 720);
        virtual ~Application();

        void Run();

        virtual void OnStart() {}
        virtual void OnUpdate(float dt) {}
        virtual void OnUIRender() {}
        virtual void OnShutdown() {}

    private:
        void RenderFrame();
        static void WindowRefreshCallback(GLFWwindow* window);
        static void WindowPosCallback(GLFWwindow* window, int x, int y);

    private:
        std::string m_Name;
        int m_Width;
        int m_Height;
        GLFWwindow* m_Window;

        // Timing
        double m_PreviousTime = 0.0;
    };

}

// To be defined by the client
Core::Application* CreateApplication();
