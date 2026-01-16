#include "Core/EntryPoint.h"

// The user application logic
class SandboxApp : public Core::Application {
public:
    SandboxApp() : Core::Application("Raylib + ImGui Hybrid Engine", 1600, 900) {}

    // Scene Resources
    RenderTexture2D m_SceneTexture = { 0 };
    int m_ViewportWidth = 0;
    int m_ViewportHeight = 0;
    Model m_CubeModel = { 0 };
    
    // Scene State
    Camera3D m_Camera = { 0 };
    float m_CubeRotation = 0.0f;
    float m_RotationSpeed = 1.0f;
    bool m_DrawWireframe = false;
    bool m_AutoRotate = true;

    // Visual Settings
    Color m_BgColor = { 25, 25, 25, 255 };
    Color m_CubeColor = { 230, 41, 55, 255 }; // Raylib Red
    Color m_GridColor = { 60, 60, 60, 255 };

    void OnStart() override {
        // Initialize Camera
        m_Camera.position = Vector3{ 4.0f, 4.0f, 4.0f };
        m_Camera.target = Vector3{ 0.0f, 0.0f, 0.0f };
        m_Camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
        m_Camera.fovy = 45.0f;
        m_Camera.projection = CAMERA_PERSPECTIVE;

        // Initialize Render Texture (start small, will resize in Update)
        m_ViewportWidth = 1280;
        m_ViewportHeight = 720;
        m_SceneTexture = LoadRenderTexture(m_ViewportWidth, m_ViewportHeight);

        // Load a Unit Cube Model
        Mesh mesh = GenMeshCube(1.5f, 1.5f, 1.5f);
        m_CubeModel = LoadModelFromMesh(mesh);
    }

    void OnUpdate(float dt) override {
        // Update Logic
        if (m_AutoRotate) {
            m_CubeRotation += (45.0f * dt * m_RotationSpeed);
            if (m_CubeRotation > 360.0f) m_CubeRotation -= 360.0f;
        }

        // --- Render Scene to Texture ---
        BeginTextureMode(m_SceneTexture);
        ClearBackground(m_BgColor);

        BeginMode3D(m_Camera);

            // Draw Grid
            DrawGrid(10, 1.0f);
            
            // Draw Axes
            DrawLine3D({0,0,0}, {1,0,0}, RED);
            DrawLine3D({0,0,0}, {0,1,0}, GREEN);
            DrawLine3D({0,0,0}, {0,0,1}, BLUE);

            // Draw Rotating Cube
            Vector3 cubePos = { 0.0f, 0.5f, 0.0f };
            Vector3 rotationAxis = { 0.0f, 1.0f, 0.0f };
            Vector3 scale = { 1.0f, 1.0f, 1.0f };

            if (m_DrawWireframe) {
                DrawModelWiresEx(m_CubeModel, cubePos, rotationAxis, m_CubeRotation, scale, m_CubeColor);
            } else {
                DrawModelEx(m_CubeModel, cubePos, rotationAxis, m_CubeRotation, scale, m_CubeColor);
                DrawModelWiresEx(m_CubeModel, cubePos, rotationAxis, m_CubeRotation, scale, BLACK);
            }

        EndMode3D();
        EndTextureMode();
    }

    void OnUIRender() override {
        // --- DockSpace ---
        ImGuiID dockSpaceId = ImGui::GetID("MyDockSpace");
        ImGui::DockSpaceOverViewport(dockSpaceId, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

        // --- Settings Panel ---
        ImGui::Begin("Settings");
        
        ImGui::TextDisabled("Performance");
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
        ImGui::Separator();

        ImGui::TextDisabled("Scene Control");
        ImGui::Checkbox("Auto Rotate", &m_AutoRotate);
        if (m_AutoRotate) {
             ImGui::SliderFloat("Speed", &m_RotationSpeed, 0.0f, 5.0f);
        } else {
             ImGui::SliderFloat("Rotation", &m_CubeRotation, 0.0f, 360.0f);
        }
        ImGui::Checkbox("Wireframe Mode", &m_DrawWireframe);
        
        ImGui::Separator();
        ImGui::TextDisabled("Colors");
        
        // Helper to convert Raylib Color to ImVec4 and back
        auto EditColor = [](const char* label, Color& c) {
            float col[4] = { c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, c.a / 255.0f };
            if (ImGui::ColorEdit4(label, col)) {
                c.r = (unsigned char)(col[0] * 255);
                c.g = (unsigned char)(col[1] * 255);
                c.b = (unsigned char)(col[2] * 255);
                c.a = (unsigned char)(col[3] * 255);
            }
        };

        EditColor("Cube Color", m_CubeColor);
        EditColor("Background", m_BgColor);

        ImGui::End();

        // --- Viewport Window ---
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Viewport");
        
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        int desiredW = (int)viewportPanelSize.x;
        int desiredH = (int)viewportPanelSize.y;

        // Resize texture if viewport changed size (and is valid)
        if (desiredW > 0 && desiredH > 0 && (desiredW != m_ViewportWidth || desiredH != m_ViewportHeight))
        {
            UnloadRenderTexture(m_SceneTexture);
            m_ViewportWidth = desiredW;
            m_ViewportHeight = desiredH;
            m_SceneTexture = LoadRenderTexture(m_ViewportWidth, m_ViewportHeight);
        }

        // Draw the texture
        // We flip the UVs (0,1) to (1,0) because Raylib renders upside down relative to ImGui/OpenGL coordinates
        ImTextureID texID = (ImTextureID)(intptr_t)m_SceneTexture.texture.id;
        ImGui::Image(texID, ImVec2((float)m_ViewportWidth, (float)m_ViewportHeight), ImVec2(0, 1), ImVec2(1, 0));

        ImGui::End();
        ImGui::PopStyleVar();
    }

    void OnShutdown() override {
        UnloadRenderTexture(m_SceneTexture);
        UnloadModel(m_CubeModel);
    }
};

Core::Application* CreateApplication() {
    return new SandboxApp();
}
