#include "GameLayer.h"
#include "Core/Renderer/AssetManager.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"

FGameLayer::FGameLayer() : Core::FLayer("GameLayer") 
{
}

void FGameLayer::OnAttach() 
{
    ActiveScene = std::make_shared<Core::FScene>();
    
    // Application-Specific Entity Setup
    Core::FEntity CubeEntity = ActiveScene->CreateEntity("Demo Cube");
    CubeEntity.AddComponent<Core::ModelComponent>(Core::AssetManager::RegisterModel("DemoCube", raylib::Model(GenMeshCube(1.5f, 1.5f, 1.5f))));
    CubeEntity.AddComponent<Core::MeshRendererComponent>(raylib::Color(230, 41, 55, 255));
    CubeEntity.GetComponent<Core::TransformComponent>().Position = { 0, 0, 0};

    Core::FEntity FloorEntity = ActiveScene->CreateEntity("Floor Plane");
    FloorEntity.AddComponent<Core::ModelComponent>(Core::AssetManager::RegisterModel("FloorPlane", raylib::Model(GenMeshPlane(10.0f, 10.0f, 10, 10))));
    FloorEntity.AddComponent<Core::MeshRendererComponent>(raylib::Color(200, 200, 200, 255));

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
        auto View = ActiveScene->GetRegistry().view<Core::TransformComponent, Core::ModelComponent>();
        for (auto Ent : View) 
        {
            auto& Transform = View.get<Core::TransformComponent>(Ent);
            auto& ModelComp = View.get<Core::ModelComponent>(Ent);
            
            if (ModelComp.Model && ModelComp.Model->IsValid()) 
            {
                raylib::Color EntColor = raylib::Color(255, 255, 255, 255);
                bool bDrawWires = false;

                if (ActiveScene->GetRegistry().all_of<Core::MeshRendererComponent>(Ent)) 
                {
                    auto& Renderer = ActiveScene->GetRegistry().get<Core::MeshRendererComponent>(Ent);
                    EntColor = Renderer.BaseColor;
                    bDrawWires = Renderer.bDrawWireframe;
                }

                if (bDrawWires)
                    ModelComp.Model->DrawWires(Transform.Position, Transform.RotationAxis, Transform.Angle, Transform.Scale, EntColor);
                else
                    ModelComp.Model->Draw(Transform.Position, Transform.RotationAxis, Transform.Angle, Transform.Scale, EntColor);
            }
        }
    }

    Camera.EndMode();
}
