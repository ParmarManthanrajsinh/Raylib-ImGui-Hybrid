#pragma once
#include "Entity.h"
#include <vector>

namespace Core 
{
    class FScene 
    {
    public:
        FScene();
        ~FScene();

        void OnUpdate(float DeltaTime);
        std::vector<FEntity>& GetEntities() { return Entities; }

    private:
        std::vector<FEntity> Entities;
    };
}
