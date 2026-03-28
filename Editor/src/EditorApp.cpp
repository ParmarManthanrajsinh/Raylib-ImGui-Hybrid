#include "Core/Application/Application.h"
#include <raylib-cpp.hpp>
#include "Core/Application/EntryPoint.h"

// ImGui Tooling Includes strictly bound to Editor target
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"

#include "ApplicationTheme.h"
#include "ApplicationLayout.h"
#include "EditorLayer.h"

class FEditorApp : public Core::FApplication 
{
public:
    FEditorApp() 
        : Core::FApplication(Core::FApplicationConfig{ .Name = "Raylib-ImGui Editor", .Width = 1600, .Height = 900 })
    {
    }

    // --- ENGINE HOOKS FOR IMGUI ---
    void OnMainThreadInit() override 
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        Core::SetApplicationTheme();
        Core::LoadApplicationDefaultIni();
        ImGui_ImplGlfw_InitForOpenGL(GetWindow(), true);
    }

    void OnRenderThreadInit() override 
    {
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    void OnPreUpdate() override 
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void OnPostRender() override 
    {
        // Actually renders ImGui Over the glClear buffer
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* BackupCurrentContext = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(BackupCurrentContext);
        }
    }

    void OnRenderThreadShutdown() override 
    {
        ImGui_ImplOpenGL3_Shutdown();
    }

    void OnMainThreadShutdown() override 
    {
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    // --- APP SETUP ---

    void OnStart() override 
    {
        PushLayer(new EditorLayer());
    }
};

Core::FApplication* CreateApplication() 
{
    return new FEditorApp();
}