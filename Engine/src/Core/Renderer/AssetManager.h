#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <raylib-cpp.hpp>

namespace Core 
{
    class AssetManager 
    {
    public:
        // Registers a uniquely named model safely into the global VRAM cache. 
        // e.g., RegisterModel("Player", raylib::Model("player.obj"));
        static std::shared_ptr<raylib::Model> RegisterModel(const std::string& Name, raylib::Model&& LoadedModel);
        
        // Returns the model securely based on its String ID. Returns nullptr if not found.
        static std::shared_ptr<raylib::Model> GetModel(const std::string& Name);

        // Required explicitly before CloseWindow() executes to safely un-bind VRAM on shutdown.
        static void Clear();

    private:
        static std::unordered_map<std::string, std::shared_ptr<raylib::Model>> ModelRegistry;
    };
}
