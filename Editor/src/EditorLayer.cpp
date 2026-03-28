#include "EditorLayer.h"
#include "GameLayer.h"
#include "Core/Application/Application.h"
#include "Scene/Components.h"
#include "imgui.h"
#include <cstring>

EditorLayer::EditorLayer() : Core::FLayer("EditorLayer") {}

void EditorLayer::OnAttach() 
{
    ViewportTexture.emplace(ViewportWidth, ViewportHeight);
    
    // We expect the Application to have pushed the GameLayer
    // In a mature godot-like engine, we'd search the LayerStack. For now, we instantiate it.
    GameLayerContext = new FGameLayer();
    Core::FApplication::Get().PushLayer(GameLayerContext);
}

void EditorLayer::OnDetach() 
{
    ViewportTexture.reset();
}

void EditorLayer::OnUpdate(float DeltaTime) 
{
    // Guard: if the GameLayer's scene was reset, our SelectedEntity's FScene* is dangling
    if (GameLayerContext && !GameLayerContext->GetScene())
        SelectedEntity = Core::FEntity{};

    if (DesiredViewportWidth > 0 && DesiredViewportHeight > 0 && 
       (DesiredViewportWidth != ViewportWidth || DesiredViewportHeight != ViewportHeight))
    {
        ViewportWidth = DesiredViewportWidth;
        ViewportHeight = DesiredViewportHeight;
        ViewportTexture.emplace(ViewportWidth, ViewportHeight);
    }
}

void EditorLayer::OnRender()
{
    // Capture the GameLayer's 3D output into our RenderTexture for ImGui display
    if (ViewportTexture.has_value() && ViewportTexture->IsValid() && GameLayerContext)
    {
        ViewportTexture->BeginMode();
        raylib::Color(30, 30, 30, 255).ClearBackground();

        GameLayerContext->RenderScene();

        ViewportTexture->EndMode();
    }
}

void EditorLayer::OnUIRender() 
{
    ImGuiID DockSpaceId = ImGui::GetID("EditorDockSpace");
    ImGui::DockSpaceOverViewport(DockSpaceId, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

    DrawHierarchyPanel();
    DrawInspectorPanel();
    DrawViewportPanel();
}

void EditorLayer::DrawHierarchyPanel()
{
    ImGui::Begin("Scene Hierarchy");
    if (GameLayerContext && GameLayerContext->GetScene())
    {
        auto& Registry = GameLayerContext->GetScene()->GetRegistry();
        Registry.view<Core::TagComponent>().each([&](auto entity, auto& tag) 
        {
            Core::FEntity e(entity, GameLayerContext->GetScene().get());
            ImGuiTreeNodeFlags flags = ((SelectedEntity == e) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_Leaf;
            bool opened = ImGui::TreeNodeEx((void*)(intptr_t)(uint32_t)entity, flags, "%s", tag.Name.c_str());
            if (ImGui::IsItemClicked()) SelectedEntity = e;
            if (opened) ImGui::TreePop();
        });
    }

    if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
        SelectedEntity = Core::FEntity{};

    ImGui::End();
}

void EditorLayer::DrawInspectorPanel()
{
    ImGui::Begin("Properties");
    if (SelectedEntity)
    {
        if (SelectedEntity.HasComponent<Core::TagComponent>())
        {
            auto& tag = SelectedEntity.GetComponent<Core::TagComponent>();
            char buffer[256];
            strncpy(buffer, tag.Name.c_str(), sizeof(buffer) - 1);
            buffer[sizeof(buffer)-1] = 0;
            if (ImGui::InputText("Name", buffer, sizeof(buffer))) tag.Name = buffer;
        }

        ImGui::Separator();

        if (SelectedEntity.HasComponent<Core::TransformComponent>())
        {
            auto& transform = SelectedEntity.GetComponent<Core::TransformComponent>();
            float pos[3] = { transform.Position.x, transform.Position.y, transform.Position.z };
            if (ImGui::DragFloat3("Position", pos, 0.1f)) transform.Position = {pos[0], pos[1], pos[2]};

            float rot[3] = { transform.RotationAxis.x, transform.RotationAxis.y, transform.RotationAxis.z };
            if (ImGui::DragFloat3("Rot Axis", rot, 0.1f)) transform.RotationAxis = {rot[0], rot[1], rot[2]};
            ImGui::DragFloat("Angle", &transform.Angle, 1.0f, 0.0f, 360.0f);

            float scale[3] = { transform.Scale.x, transform.Scale.y, transform.Scale.z };
            if (ImGui::DragFloat3("Scale", scale, 0.1f)) transform.Scale = {scale[0], scale[1], scale[2]};
        }

        ImGui::Separator();

        if (SelectedEntity.HasComponent<Core::MeshRendererComponent>())
        {
            auto& renderer = SelectedEntity.GetComponent<Core::MeshRendererComponent>();
            float col[4] = { renderer.BaseColor.r / 255.0f, renderer.BaseColor.g / 255.0f, renderer.BaseColor.b / 255.0f, renderer.BaseColor.a / 255.0f };
            if (ImGui::ColorEdit4("Color", col)) 
            {
                renderer.BaseColor.r = static_cast<unsigned char>(col[0] * 255);
                renderer.BaseColor.g = static_cast<unsigned char>(col[1] * 255);
                renderer.BaseColor.b = static_cast<unsigned char>(col[2] * 255);
                renderer.BaseColor.a = static_cast<unsigned char>(col[3] * 255);
            }
            ImGui::Checkbox("Wireframe", &renderer.bDrawWireframe);
        }
    }
    else
    {
        ImGui::TextDisabled("Select an entity to view properties.");
    }
    ImGui::End();
}

void EditorLayer::DrawViewportPanel()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport");

    ImVec2 ViewportPanelSize = ImGui::GetContentRegionAvail();
    DesiredViewportWidth = static_cast<int>(ViewportPanelSize.x);
    DesiredViewportHeight = static_cast<int>(ViewportPanelSize.y);

    if (ViewportTexture.has_value() && ViewportTexture->IsValid())
    {
        ImTextureID TexID = (ImTextureID)(intptr_t)ViewportTexture->GetTexture().id;
        ImGui::Image(TexID, ImVec2(static_cast<float>(ViewportWidth), static_cast<float>(ViewportHeight)), ImVec2(0, 1), ImVec2(1, 0));
    }

    ImGui::End();
    ImGui::PopStyleVar();
}
