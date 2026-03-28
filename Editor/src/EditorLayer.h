#pragma once
#include "Core/Layers/Layer.h"
#include "Scene/Scene.h"
#include <raylib-cpp.hpp>
#include <optional>
#include <vector>
#include <string>
#include <memory>

class FGameLayer;

class EditorLayer : public Core::FLayer {
public:
    EditorLayer();
    virtual ~EditorLayer() = default;

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float DeltaTime) override;
    void OnUIRender() override;

private:
    void DrawHierarchyPanel();
    void DrawInspectorPanel();
    void DrawViewportPanel();

private:
    std::optional<raylib::RenderTexture2D> ViewportTexture;
    raylib::Camera3D Camera;

    int ViewportWidth = 1280;
    int ViewportHeight = 720;
    int DesiredViewportWidth = 1280;
    int DesiredViewportHeight = 720;

    class FGameLayer* GameLayerContext = nullptr;
    int SelectedEntityIndex = -1;
};
