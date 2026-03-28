#include "Scene.h"

namespace Core 
{
    FScene::FScene() 
    {
    }

    FScene::~FScene() 
    {
        for (auto& Ent : Entities) {
            Ent.Model.reset();
        }
    }

    void FScene::OnUpdate(float DeltaTime) 
    {
        // Entity Physics & Component Logic Updates should go here
    }
}
