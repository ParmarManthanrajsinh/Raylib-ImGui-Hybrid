#pragma once

#include "raylib.h"
#include <string>

namespace Core {

    class ModelLoader
    {
    public:
        // Returns an invalid model (meshCount == 0) on failure
        static Model LoadModel(const std::string& Path);
        
    private:
        static Model LoadFBX(const std::string& Path);
    };

}
