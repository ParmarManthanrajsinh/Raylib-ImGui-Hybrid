#pragma once
#include <Core/Layers/Layer.h>
#include <Scene/Scene.h>
#include <raylib-cpp.hpp>
#include <memory>

class FGameLayer : public Core::FLayer 
{
public:
    FGameLayer();
    ~FGameLayer() override = default;

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float DeltaTime) override;
    
    // Explicit scene rendering path so the Editor can redirect it to a Framebuffer
    void RenderScene();

    std::shared_ptr<Core::FScene> GetScene() const { return ActiveScene; }
    raylib::Camera3D& GetCamera() { return Camera; }

private:
    std::shared_ptr<Core::FScene> ActiveScene;
    raylib::Camera3D Camera;
};
