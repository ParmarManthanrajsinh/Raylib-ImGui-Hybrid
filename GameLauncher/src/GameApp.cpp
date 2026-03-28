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

    void OnUpdate(float DeltaTime) override 
    {
    }

    void OnUIRender() override
    {
        // Must execute natively within the Application's BeginDrawing() block!
        GameLayer->RenderScene();
    }
};

Core::FApplication* CreateApplication() 
{
    return new FGameApp();
}
