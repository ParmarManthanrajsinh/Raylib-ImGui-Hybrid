#include "AssetManager.h"
#include "Core/Logging/Log.h"

namespace Core 
{
    std::unordered_map<std::string, std::shared_ptr<raylib::Model>> AssetManager::ModelRegistry;

    std::shared_ptr<raylib::Model> AssetManager::RegisterModel(const std::string& Name, raylib::Model&& LoadedModel)
    {
        auto It = ModelRegistry.find(Name);
        if (It != ModelRegistry.end())
        {
            FLog::CoreWarn("AssetManager: Model '{0}' is already registered!", Name);
            return It->second;
        }

        // Move the physical raylib model into a shared pointer memory block
        auto SharedModel = std::make_shared<raylib::Model>(std::move(LoadedModel));
        ModelRegistry[Name] = SharedModel;

        FLog::CoreDebug("AssetManager: Successfully registered model '{0}'", Name);
        return SharedModel;
    }

    std::shared_ptr<raylib::Model> AssetManager::GetModel(const std::string& Name)
    {
        auto It = ModelRegistry.find(Name);
        if (It != ModelRegistry.end())
        {
            return It->second;
        }

        FLog::CoreError("AssetManager: Could not find model '{0}'", Name);
        return nullptr;
    }

    void AssetManager::Clear()
    {
        // Forcing a reset explicitly unloads the raylib::Models from VRAM.
        ModelRegistry.clear();
        FLog::CoreDebug("AssetManager: Cleared all loaded VRAM assets natively.");
    }
}
