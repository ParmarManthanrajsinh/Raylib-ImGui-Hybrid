#include "Scene.h"
#include "Entity.h"
#include "Components.h"

namespace Core 
{
    FScene::FScene() 
    {
    }

    FScene::~FScene() 
    {
    }

    FEntity FScene::CreateEntity(const std::string& name)
    {
        FEntity entity = { Registry.create(), this };
        entity.AddComponent<TransformComponent>();
        auto& tag = entity.AddComponent<TagComponent>();
        tag.Name = name.empty() ? "Entity" : name;
        return entity;
    }

    void FScene::DestroyEntity(FEntity entity)
    {
        Registry.destroy(entity);
    }

    void FScene::OnUpdate(float DeltaTime) 
    {
        // Entity Physics & Component Logic Iterations
    }
}
