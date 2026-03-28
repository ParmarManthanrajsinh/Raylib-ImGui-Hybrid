#pragma once

#include "Core/Base/Core.h"
#include "Layer.h"

#include <vector>
#include <memory>

namespace Core {

    class FLayerStack
    {
    public:
        FLayerStack() = default;
        ~FLayerStack();

        // Takes ownership of the layer. Returns a non-owning pointer for the caller to cache.
        FLayer* PushLayer(FLayer* InLayer);
        FLayer* PushOverlay(FLayer* InOverlay);
        void PopLayer(FLayer* InLayer);
        void PopOverlay(FLayer* InOverlay);

        // Iteration — yields raw pointers (non-owning views)
        std::vector<std::unique_ptr<FLayer>>::iterator begin() { return Layers.begin(); }
        std::vector<std::unique_ptr<FLayer>>::iterator end() { return Layers.end(); }
        std::vector<std::unique_ptr<FLayer>>::reverse_iterator rbegin() { return Layers.rbegin(); }
        std::vector<std::unique_ptr<FLayer>>::reverse_iterator rend() { return Layers.rend(); }

        std::vector<std::unique_ptr<FLayer>>::const_iterator begin() const { return Layers.begin(); }
        std::vector<std::unique_ptr<FLayer>>::const_iterator end() const { return Layers.end(); }
        std::vector<std::unique_ptr<FLayer>>::const_reverse_iterator rbegin() const { return Layers.rbegin(); }
        std::vector<std::unique_ptr<FLayer>>::const_reverse_iterator rend() const { return Layers.rend(); }
    private:
        std::vector<std::unique_ptr<FLayer>> Layers;
        unsigned int LayerInsertIndex = 0;
    };

}
