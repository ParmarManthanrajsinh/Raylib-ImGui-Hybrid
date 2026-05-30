#include "Application.h"
#include <memory>
    
std::unique_ptr<Core::FApplication> CreateApplication();

int main([[maybe_unused]]int argc, [[maybe_unused]] char** argv)
{
    auto App = CreateApplication();
    if(App) App->Run();
    return 0;
}
