#pragma once

#include <raylib-cpp.hpp>

// Game-specific ECS components.
// These define behaviors unique to THIS game, not the Engine.

struct RotatorComponent
{
    float Speed = 90.0f;  // Degrees per second

    RotatorComponent() = default;
    RotatorComponent(float speed) : Speed(speed) {}
};

struct BobComponent
{
    float Amplitude = 0.5f;   // How high it bobs
    float Frequency = 2.0f;   // Oscillations per second
    float BaseY = 0.0f;       // Original Y position
    float Phase = 0.0f;       // Current phase accumulator

    BobComponent() = default;
    BobComponent(float amp, float freq, float baseY = 0.0f)
        : Amplitude(amp), Frequency(freq), BaseY(baseY) {}
};
