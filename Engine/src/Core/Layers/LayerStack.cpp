#include "LayerStack.h"
#include <algorithm>

namespace Core {

    FLayerStack::~FLayerStack()
    {
        for (auto& Layer : Layers)
        {
            Layer->OnDetach();
        }
        // unique_ptrs auto-delete when the vector is destroyed
    }

    FLayer* FLayerStack::PushLayer(FLayer* InLayer)
    {
        Layers.emplace(Layers.begin() + LayerInsertIndex, std::unique_ptr<FLayer>(InLayer));
        LayerInsertIndex++;
        InLayer->OnAttach();
        return InLayer;
    }

    FLayer* FLayerStack::PushOverlay(FLayer* InOverlay)
    {
        Layers.emplace_back(std::unique_ptr<FLayer>(InOverlay));
        InOverlay->OnAttach();
        return InOverlay;
    }

    void FLayerStack::PopLayer(FLayer* InLayer)
    {
        auto it = std::find_if(Layers.begin(), Layers.begin() + LayerInsertIndex,
            [InLayer](const std::unique_ptr<FLayer>& ptr) { return ptr.get() == InLayer; });
        if (it != Layers.begin() + LayerInsertIndex)
        {
            (*it)->OnDetach();
            Layers.erase(it);  // unique_ptr destructor deletes the layer
            LayerInsertIndex--;
        }
    }

    void FLayerStack::PopOverlay(FLayer* InOverlay)
    {
        auto it = std::find_if(Layers.begin() + LayerInsertIndex, Layers.end(),
            [InOverlay](const std::unique_ptr<FLayer>& ptr) { return ptr.get() == InOverlay; });
        if (it != Layers.end())
        {
            (*it)->OnDetach();
            Layers.erase(it);  // unique_ptr destructor deletes the layer
        }
    }

}
