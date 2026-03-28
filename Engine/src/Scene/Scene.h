#pragma once
#include "entt/entt.hpp"
#include <string>

namespace Core 
{
    class FEntity;

    class FScene 
    {
    public:
        FScene();
        ~FScene();

        FEntity CreateEntity(const std::string& Name = std::string());
        void DestroyEntity(FEntity entity);

        void OnUpdate(float DeltaTime);

        entt::registry& GetRegistry() { return Registry; }

    private:
        entt::registry Registry;
    };
}
