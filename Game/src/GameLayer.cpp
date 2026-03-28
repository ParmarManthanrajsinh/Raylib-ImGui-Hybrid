#include "GameLayer.h"
#include "Core/Renderer/AssetManager.h"

FGameLayer::FGameLayer() : Core::FLayer("GameLayer") 
{
}

void FGameLayer::OnAttach() 
{
    ActiveScene = std::make_shared<Core::FScene>();
    
    // Application-Specific Entity Setup
    Core::FEntity CubeEntity;
    CubeEntity.Name = "Demo Cube";
    CubeEntity.Model = Core::AssetManager::RegisterModel("DemoCube", raylib::Model(GenMeshCube(1.5f, 1.5f, 1.5f)));
    CubeEntity.EntColor = raylib::Color(230, 41, 55, 255);
    CubeEntity.Transform.Position = { 0, 0, 0};
    ActiveScene->GetEntities().push_back(std::move(CubeEntity));

    Core::FEntity FloorEntity;
    FloorEntity.Name = "Floor Plane";
    FloorEntity.Model = Core::AssetManager::RegisterModel("FloorPlane", raylib::Model(GenMeshPlane(10.0f, 10.0f, 10, 10)));
    FloorEntity.EntColor = raylib::Color(200, 200, 200, 255);
    ActiveScene->GetEntities().push_back(std::move(FloorEntity));

    Camera.position = raylib::Vector3(5.0f, 5.0f, 5.0f);
    Camera.target = raylib::Vector3(0.0f, 0.0f, 0.0f);
    Camera.up = raylib::Vector3(0.0f, 1.0f, 0.0f);
    Camera.fovy = 45.0f;
    Camera.projection = CAMERA_PERSPECTIVE;
}

void FGameLayer::OnDetach() 
{
    ActiveScene.reset();
}

void FGameLayer::OnUpdate(float DeltaTime) 
{
    // E.g. Ticking physics, scripts, entities natively
    if (ActiveScene) 
        ActiveScene->OnUpdate(DeltaTime);
}

void FGameLayer::RenderScene()
{
    // Pure drawing logic. No clear calls here (Handled by Engine viewport or Editor Framebuffer)
    Camera.BeginMode();
    DrawGrid(20, 1.0f);

    if (ActiveScene) 
    {
        for (auto& Ent : ActiveScene->GetEntities()) 
        {
            Ent.Draw();
        }
    }

    Camera.EndMode();
}
