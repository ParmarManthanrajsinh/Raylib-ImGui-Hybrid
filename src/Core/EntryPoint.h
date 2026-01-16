#pragma once
#include "Application.h"

extern Core::Application* CreateApplication();

int main(int argc, char** argv)
{
    Core::Application* app = CreateApplication();
    app->Run();
    delete app;
    return 0;
}
