#include "Core/Application/Application.h"
#include <raylib-cpp.hpp>
#include <optional>
#include "Core/Application/EntryPoint.h"

// The user application logic
class FSandboxApp : public Core::FApplication 
{
public:
    FSandboxApp() 
        : Core::FApplication(Core::FApplicationConfig{ .Name = "Raylib + ImGui Hybrid Engine", .Width = 1600, .Height = 900 })
    {
    }

    // Scene Resources
    std::optional<raylib::RenderTexture2D> SceneTexture;
    std::optional<raylib::Model> CubeModel;
    
    // Scene State
    raylib::Camera3D Camera;
    int ViewportWidth = 0;
    int ViewportHeight = 0;
    
    // Sync UI to Render
    int DesiredViewportWidth = 1280;
    int DesiredViewportHeight = 720;

    float CubeRotation = 0.0f;
    float RotationSpeed = 1.0f;
    bool bDrawWireframe = false;
    bool bAutoRotate = true;

    // Visual Settings
    raylib::Color BgColor = raylib::Color(25, 25, 25, 255);
    raylib::Color CubeColor = raylib::Color(230, 41, 55, 255); 
    raylib::Color GridColor = raylib::Color(60, 60, 60, 255);

    void OnStart() override 
    {
        // Initialize Camera
        Camera.position = raylib::Vector3(4.0f, 4.0f, 4.0f);
        Camera.target = raylib::Vector3(0.0f, 0.0f, 0.0f);
        Camera.up = raylib::Vector3(0.0f, 1.0f, 0.0f);
        Camera.fovy = 45.0f;
        Camera.projection = CAMERA_PERSPECTIVE;

        // Initialize Render Texture
        ViewportWidth = DesiredViewportWidth;
        ViewportHeight = DesiredViewportHeight;
        SceneTexture.emplace(ViewportWidth, ViewportHeight);

        // Load a Unit Cube Model
        Mesh CubeMesh = GenMeshCube(1.5f, 1.5f, 1.5f);
        CubeModel.emplace(CubeMesh);
    }

    void OnUpdate(float DeltaTime) override 
    {
        // --- Resource Management (Pre-Render) ---
        // Resize texture if requested by UI
        if (DesiredViewportWidth > 0 && DesiredViewportHeight > 0 && 
           (DesiredViewportWidth != ViewportWidth || DesiredViewportHeight != ViewportHeight))
        {
            ViewportWidth = DesiredViewportWidth;
            ViewportHeight = DesiredViewportHeight;
            SceneTexture.emplace(ViewportWidth, ViewportHeight);
        }

        // --- Update Logic ---
        if (bAutoRotate) 
        {
            CubeRotation += (45.0f * DeltaTime * RotationSpeed);
            if (CubeRotation > 360.0f) CubeRotation -= 360.0f;
        }

        // --- Render Scene to Texture ---
        if (SceneTexture.has_value() && SceneTexture->IsValid())
        {
            SceneTexture->BeginMode();
            BgColor.ClearBackground();

            Camera.BeginMode();

                // Draw Grid
                DrawGrid(10, 1.0f);
                
                // Draw Axes
                DrawLine3D({0,0,0}, {1,0,0}, RED);
                DrawLine3D({0,0,0}, {0,1,0}, GREEN);
                DrawLine3D({0,0,0}, {0,0,1}, BLUE);

                // Draw Rotating Cube
                const raylib::Vector3 CubePos(0.0f, 0.5f, 0.0f);
                const raylib::Vector3 RotationAxis(0.0f, 1.0f, 0.0f);
                const raylib::Vector3 Scale(1.0f, 1.0f, 1.0f);

                if (bDrawWireframe) 
                {
                    CubeModel->DrawWires(CubePos, RotationAxis, CubeRotation, Scale, CubeColor);
                } 
                else 
                {
                    CubeModel->Draw(CubePos, RotationAxis, CubeRotation, Scale, CubeColor);
                    CubeModel->DrawWires(CubePos, RotationAxis, CubeRotation, Scale, BLACK);
                }

            Camera.EndMode();
            SceneTexture->EndMode();
        }
    }

    void OnUIRender() override 
    {
        // --- DockSpace ---
        ImGuiID DockSpaceId = ImGui::GetID("MyDockSpace");
        ImGui::DockSpaceOverViewport
        (
            DockSpaceId, 
            ImGui::GetMainViewport(), 
            ImGuiDockNodeFlags_PassthruCentralNode
        );

        // --- Settings Panel ---
        ImGui::Begin("Settings");
        
        ImGui::TextDisabled("Performance");
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
        ImGui::Separator();

        ImGui::TextDisabled("Scene Control");
        ImGui::Checkbox("Auto Rotate", &bAutoRotate);
        if (bAutoRotate) 
        {
             ImGui::SliderFloat("Speed", &RotationSpeed, 0.0f, 5.0f);
        } else 
        {
             ImGui::SliderFloat("Rotation", &CubeRotation, 0.0f, 360.0f);
        }
        ImGui::Checkbox("Wireframe Mode", &bDrawWireframe);
        
        ImGui::Separator();
        ImGui::TextDisabled("Colors");
        
        auto EditColor = [](const char* Label, raylib::Color& C) 
        {
            float Col[4] = { C.r / 255.0f, C.g / 255.0f, C.b / 255.0f, C.a / 255.0f };
            if (ImGui::ColorEdit4(Label, Col)) 
            {
                C.r = static_cast<unsigned char>(Col[0] * 255);
                C.g = static_cast<unsigned char>(Col[1] * 255);
                C.b = static_cast<unsigned char>(Col[2] * 255);
                C.a = static_cast<unsigned char>(Col[3] * 255);
            }
        };

        EditColor("Cube Color", CubeColor);
        EditColor("Background", BgColor);

        ImGui::End();

        // --- Viewport Window ---
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Viewport");
        
        // Read available size
        ImVec2 ViewportPanelSize = ImGui::GetContentRegionAvail();
        DesiredViewportWidth = static_cast<int>(ViewportPanelSize.x);
        DesiredViewportHeight = static_cast<int>(ViewportPanelSize.y);

        // Draw the texture
        if (SceneTexture.has_value() && SceneTexture->IsValid())
        {
            // We flip the UVs (0,1) to (1,0) because Raylib renders upside down relative to ImGui/OpenGL coordinates
            ImTextureID TexID = (ImTextureID)(intptr_t)SceneTexture->GetTexture().id;
            ImGui::Image
            (
                TexID, 
                ImVec2
                (
                    static_cast<float>(ViewportWidth), 
                    static_cast<float>(ViewportHeight)
                ), 
                ImVec2(0, 1), 
                ImVec2(1, 0)
            );
        }

        ImGui::End();
        ImGui::PopStyleVar();
    }

    void OnShutdown() override 
    {
        // Must clear RAII resources while OpenGL context is still active on this thread!
        SceneTexture.reset();
        CubeModel.reset();
    }
};

Core::FApplication* CreateApplication() 
{
    return new FSandboxApp();
}