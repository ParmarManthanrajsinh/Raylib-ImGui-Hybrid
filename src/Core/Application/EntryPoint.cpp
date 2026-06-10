#include "EntryPoint.h"
#include "Application.h"

int main(int, char**)
{
    auto App = CreateApplication();
    App->Run();
    return 0;
}