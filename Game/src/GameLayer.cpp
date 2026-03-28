#include "GameLayer.h"
#include "GameComponents.h"
#include "Core/Renderer/AssetManager.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"
#include <cmath>

FGameLayer::FGameLayer() : Core::FLayer("GameLayer") 
{
}

void FGameLayer::OnAttach() 
{
    ActiveScene = std::make_shared<Core::FScene>();
    
    // ===================================================================
    //  ECS DEMO SCENE
    //  Each entity demonstrates a different component combination.
    //  Systems in FScene::OnUpdate only touch entities with matching components.
    // ===================================================================

    // --- 1. Rotating Red Cube (Transform + Model + Renderer + Rotator) ---
    // This entity spins because it has a RotatorComponent.
    {
        Core::FEntity Cube = ActiveScene->CreateEntity("Spinning Cube");
        Cube.AddComponent<Core::ModelComponent>(
            Core::AssetManager::RegisterModel("Cube", raylib::Model(GenMeshCube(1.2f, 1.2f, 1.2f))));
        Cube.AddComponent<Core::MeshRendererComponent>(raylib::Color(230, 41, 55, 255));
        Cube.AddComponent<RotatorComponent>(120.0f);
        Cube.GetComponent<Core::TransformComponent>().Position = {0.0f, 1.0f, 0.0f};
    }

    // --- 2. Bobbing Blue Sphere (Transform + Model + Renderer + Bob) ---
    // This entity floats up and down because it has a BobComponent.
    {
        Core::FEntity Sphere = ActiveScene->CreateEntity("Bobbing Sphere");
        Sphere.AddComponent<Core::ModelComponent>(
            Core::AssetManager::RegisterModel("Sphere", raylib::Model(GenMeshSphere(0.6f, 16, 16))));
        Sphere.AddComponent<Core::MeshRendererComponent>(raylib::Color(41, 121, 230, 255));
        Sphere.AddComponent<BobComponent>(0.8f, 1.5f, 2.0f);
        Sphere.GetComponent<Core::TransformComponent>().Position = {-3.0f, 2.0f, 0.0f};
    }

    // --- 3. Spinning + Bobbing Torus (Transform + Model + Renderer + Rotator + Bob) ---
    // This entity has BOTH behaviors — it spins AND bobs simultaneously.
    {
        Core::FEntity Torus = ActiveScene->CreateEntity("Spinning Bobbing Torus");
        Torus.AddComponent<Core::ModelComponent>(
            Core::AssetManager::RegisterModel("Torus", raylib::Model(GenMeshKnot(0.5f, 0.2f, 16, 128))));
        Torus.AddComponent<Core::MeshRendererComponent>(raylib::Color(255, 161, 0, 255));
        Torus.AddComponent<RotatorComponent>(200.0f);
        Torus.AddComponent<BobComponent>(0.5f, 1.0f, 1.5f);
        Torus.GetComponent<Core::TransformComponent>().Position = {3.0f, 1.5f, 0.0f};
    }

    // --- 4. Static Green Cylinder (Transform + Model + Renderer, NO behavior) ---
    // This entity has NO RotatorComponent or BobComponent,
    // so it sits completely still — proving Systems skip it.
    {
        Core::FEntity Cylinder = ActiveScene->CreateEntity("Static Cylinder");
        Cylinder.AddComponent<Core::ModelComponent>(
            Core::AssetManager::RegisterModel("Cylinder", raylib::Model(GenMeshCylinder(0.5f, 2.0f, 12))));
        Cylinder.AddComponent<Core::MeshRendererComponent>(raylib::Color(0, 200, 80, 255));
        Cylinder.GetComponent<Core::TransformComponent>().Position = {0.0f, 1.0f, -3.0f};
    }

    // --- 5. Wireframe Rotating Cube (Transform + Model + Renderer[wireframe] + Rotator) ---
    // Demonstrates the wireframe toggle via MeshRendererComponent.
    {
        Core::FEntity Wire = ActiveScene->CreateEntity("Wireframe Cube");
        Wire.AddComponent<Core::ModelComponent>(
            Core::AssetManager::RegisterModel("WireCube", raylib::Model(GenMeshCube(1.8f, 1.8f, 1.8f))));
        auto& Renderer = Wire.AddComponent<Core::MeshRendererComponent>(raylib::Color(180, 180, 255, 255));
        Renderer.bDrawWireframe = true;
        Wire.AddComponent<RotatorComponent>(45.0f);
        Wire.GetComponent<Core::TransformComponent>().Position = {0.0f, 1.0f, 3.0f};
    }

    // --- 6. Floor Plane (Transform + Model + Renderer, static ground) ---
    {
        Core::FEntity Floor = ActiveScene->CreateEntity("Floor");
        Floor.AddComponent<Core::ModelComponent>(
            Core::AssetManager::RegisterModel("Floor", raylib::Model(GenMeshPlane(20.0f, 20.0f, 10, 10))));
        Floor.AddComponent<Core::MeshRendererComponent>(raylib::Color(80, 80, 80, 255));
    }

    // Camera
    Camera.position = raylib::Vector3(8.0f, 6.0f, 8.0f);
    Camera.target = raylib::Vector3(0.0f, 1.0f, 0.0f);
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
    if (!ActiveScene) return;

    // Let Engine-level systems run first
    ActiveScene->OnUpdate(DeltaTime);

    auto& Registry = ActiveScene->GetRegistry();

    // --- Game System: Rotation ---
    {
        auto View = Registry.view<Core::TransformComponent, RotatorComponent>();
        for (auto Entity : View)
        {
            auto& Transform = View.get<Core::TransformComponent>(Entity);
            auto& Rotator   = View.get<RotatorComponent>(Entity);

            Transform.Angle += Rotator.Speed * DeltaTime;
            if (Transform.Angle > 360.0f) Transform.Angle -= 360.0f;
        }
    }

    // --- Game System: Bobbing ---
    {
        auto View = Registry.view<Core::TransformComponent, BobComponent>();
        for (auto Entity : View)
        {
            auto& Transform = View.get<Core::TransformComponent>(Entity);
            auto& Bob       = View.get<BobComponent>(Entity);

            Bob.Phase += DeltaTime * Bob.Frequency * 2.0f * 3.14159265f;
            Transform.Position.y = Bob.BaseY + sinf(Bob.Phase) * Bob.Amplitude;
        }
    }
}

void FGameLayer::RenderScene()
{
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
