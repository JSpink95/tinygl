#pragma once

#include <array>
#include "maths/maths.h"

namespace glfw
{
    struct window;
}

struct cursor_mode
{
    enum enum_ { free, locked, max };
};

struct input_t
{
    static input_t* instance;
    static constexpr const unsigned int MaxKeys = 512;

    std::array<bool, MaxKeys> previousKeyStates;
    std::array<bool, MaxKeys> currentKeyStates;

    cursor_mode::enum_ cursorMode = cursor_mode::free;
    float2 lastCursor = float2(0.0f);
    float2 cursor = float2(0.0f);
};

void CreateInput();
void DestroyInput();

bool IsKeyDown(unsigned int key);
bool QueryKey(unsigned int key, bool& justPressed, bool& justReleased);

float2 GetCursorDelta();

void UpdateInput(glfw::window* window);
