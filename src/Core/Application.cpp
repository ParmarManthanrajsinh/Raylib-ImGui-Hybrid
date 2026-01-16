#include "Application.h"

#include <iostream>
#include <glad/glad.h>
#include "GLFW/glfw3.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

extern "C" {
    #include "rlgl.h"
}

namespace Core {

    Application::Application(const std::string& name, int width, int height)
        : m_Name(name), m_Width(width), m_Height(height), m_Window(nullptr)
    {
    }

    Application::~Application()
    {
    }

    void Application::Run()
    {
        if (!glfwInit())
        {
            std::cerr << "Failed to init GLFW\n";
            return;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #if __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

        m_Window = glfwCreateWindow(m_Width, m_Height, m_Name.c_str(), nullptr, nullptr);
        if (!m_Window)
        {
            std::cerr << "Failed to create GLFW window\n";
            glfwTerminate();
            return;
        }

        glfwMakeContextCurrent(m_Window);
        glfwSwapInterval(1);

        // Required for the refresh callback to access 'this'
        glfwSetWindowUserPointer(m_Window, this);
        glfwSetWindowRefreshCallback(m_Window, WindowRefreshCallback);
        glfwSetWindowPosCallback(m_Window, WindowPosCallback);

        rlLoadExtensions((void*)glfwGetProcAddress);

        // Initialize ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& IO = ImGui::GetIO(); (void)IO;
        IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        IO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
        ImGui_ImplOpenGL3_Init("#version 330");

        // Initialize Raylib's RLGL
        rlglInit(m_Width, m_Height);

        // User Start
        OnStart();

        // Timing
        m_PreviousTime = glfwGetTime();

        while (!glfwWindowShouldClose(m_Window))
        {
            glfwPollEvents();
            RenderFrame();
            glfwSwapBuffers(m_Window);
        }

        // User Shutdown
        OnShutdown();

        // Cleanup
        rlglClose();

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    void Application::RenderFrame()
    {
        double CurrentTime = glfwGetTime();
        float DeltaSeconds = (float)(CurrentTime - m_PreviousTime);
        m_PreviousTime = CurrentTime;

        if (DeltaSeconds <= 0.0f) 
            DeltaSeconds = 1.0f / 60.0f;

        // User Update
        OnUpdate(DeltaSeconds);

        // ImGui Frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // User UI Render
        OnUIRender();

        // Render Frame
        ImGui::Render();

        int FrameBufferW, FrameBufferH;
        glfwGetFramebufferSize(m_Window, &FrameBufferW, &FrameBufferH);
        glViewport(0, 0, FrameBufferW, FrameBufferH);
        glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* BackupCurrentContext = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(BackupCurrentContext);
        }
    }

    void Application::WindowRefreshCallback(GLFWwindow* window)
    {
        Application* app = (Application*)glfwGetWindowUserPointer(window);
        if (app)
        {
            app->RenderFrame();
            glfwSwapBuffers(window);
        }
    }

    void Application::WindowPosCallback(GLFWwindow* window, int x, int y)
    {
        Application* app = (Application*)glfwGetWindowUserPointer(window);
        if (app)
        {
            app->RenderFrame();
            glfwSwapBuffers(window);
        }
    }


}
