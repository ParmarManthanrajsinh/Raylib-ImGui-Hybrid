#include "EditorLayer.h"
#include "GameLayer.h"
#include "Core/Application/Application.h"
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
    // The GameLayer is in the Application LayerStack, so it gets OnUpdate() ticked automatically.
    // EditorLayer handles sizing the viewport and capturing the GameLayer's output!

    if (DesiredViewportWidth > 0 && DesiredViewportHeight > 0 && 
       (DesiredViewportWidth != ViewportWidth || DesiredViewportHeight != ViewportHeight))
    {
        ViewportWidth = DesiredViewportWidth;
        ViewportHeight = DesiredViewportHeight;
        ViewportTexture.emplace(ViewportWidth, ViewportHeight);
    }

    if (ViewportTexture.has_value() && ViewportTexture->IsValid() && GameLayerContext)
    {
        ViewportTexture->BeginMode();
        raylib::Color(30, 30, 30, 255).ClearBackground();

        // Let the Game render its state visually directly into our ImGui Viewport frame!
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
        auto& Entities = GameLayerContext->GetScene()->GetEntities();
        for (int i = 0; i < Entities.size(); ++i)
        {
            ImGuiTreeNodeFlags flags = ((SelectedEntityIndex == i) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_Leaf;
            bool opened = ImGui::TreeNodeEx((void*)(intptr_t)i, flags, "%s", Entities[i].Name.c_str());
            if (ImGui::IsItemClicked()) SelectedEntityIndex = i;
            if (opened) ImGui::TreePop();
        }
    }

    if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
        SelectedEntityIndex = -1;

    ImGui::End();
}

void EditorLayer::DrawInspectorPanel()
{
    ImGui::Begin("Properties");
    if (GameLayerContext && GameLayerContext->GetScene() && SelectedEntityIndex != -1 && SelectedEntityIndex < GameLayerContext->GetScene()->GetEntities().size())
    {
        auto& Ent = GameLayerContext->GetScene()->GetEntities()[SelectedEntityIndex];
        char buffer[256];
        strncpy(buffer, Ent.Name.c_str(), sizeof(buffer) - 1);
        buffer[sizeof(buffer)-1] = 0;
        
        if (ImGui::InputText("Name", buffer, sizeof(buffer))) Ent.Name = buffer;

        ImGui::Separator();
        float pos[3] = { Ent.Transform.Position.x, Ent.Transform.Position.y, Ent.Transform.Position.z };
        if (ImGui::DragFloat3("Position", pos, 0.1f)) Ent.Transform.Position = {pos[0], pos[1], pos[2]};

        float rot[3] = { Ent.Transform.RotationAxis.x, Ent.Transform.RotationAxis.y, Ent.Transform.RotationAxis.z };
        if (ImGui::DragFloat3("Rot Axis", rot, 0.1f)) Ent.Transform.RotationAxis = {rot[0], rot[1], rot[2]};
        ImGui::DragFloat("Angle", &Ent.Transform.Angle, 1.0f, 0.0f, 360.0f);

        float scale[3] = { Ent.Transform.Scale.x, Ent.Transform.Scale.y, Ent.Transform.Scale.z };
        if (ImGui::DragFloat3("Scale", scale, 0.1f)) Ent.Transform.Scale = {scale[0], scale[1], scale[2]};

        ImGui::Separator();
        float col[4] = { Ent.EntColor.r / 255.0f, Ent.EntColor.g / 255.0f, Ent.EntColor.b / 255.0f, Ent.EntColor.a / 255.0f };
        if (ImGui::ColorEdit4("Color", col)) 
        {
            Ent.EntColor.r = static_cast<unsigned char>(col[0] * 255);
            Ent.EntColor.g = static_cast<unsigned char>(col[1] * 255);
            Ent.EntColor.b = static_cast<unsigned char>(col[2] * 255);
            Ent.EntColor.a = static_cast<unsigned char>(col[3] * 255);
        }

        ImGui::Checkbox("Wireframe", &Ent.bDrawWireframe);
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
