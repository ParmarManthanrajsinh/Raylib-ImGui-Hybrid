#include "Input.h"
#include <raylib-cpp.hpp>

namespace Core {

    bool FInput::IsKeyPressed(int KeyCode)
    {
        return IsKeyDown(KeyCode); // Raylib maps directly to standard keycodes for the most part
    }

    bool FInput::IsMouseButtonPressed(int Button)
    {
        return IsMouseButtonDown(Button);
    }

    std::pair<float, float> FInput::GetMousePosition()
    {
        raylib::Vector2 Pos = ::GetMousePosition();
        return { Pos.x, Pos.y };
    }

    float FInput::GetMouseX()
    {
        return ::GetMouseX();
    }

    float FInput::GetMouseY()
    {
        return ::GetMouseY();
    }

}
