#include "Core/Application/Application.h"
#include <memory>
#include "Core/Application/EntryPoint.h"
#include "GameLayer.h"

class FGameApp : public Core::FApplication 
{
public:
    FGameApp() 
        : Core::FApplication(Core::FApplicationConfig{ .Name = "Raylib-ImGui Engine Demo Game", .Width = 1280, .Height = 720 })
    {
    }

    FGameLayer* GameLayer;

    void OnStart() override 
    {
        GameLayer = new FGameLayer();
        PushLayer(GameLayer);
    }

    void OnRender() override
    {
        // 3D scene rendering belongs in the render pass, not the UI pass
        GameLayer->RenderScene();
    }
};

Core::FApplication* CreateApplication() 
{
    return new FGameApp();
}
