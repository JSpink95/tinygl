#include "engine/input.h"

#include "gl/window.h"
#include "glfw/glfw.h"

input* input::instance = nullptr;

void CreateInput()
{
    input::instance = new input;

    std::fill(input::instance->currentKeyStates.begin(), input::instance->currentKeyStates.end(), false);
    input::instance->previousKeyStates = input::instance->currentKeyStates;
}

void DestroyInput()
{
    if (input::instance)
    {
        delete input::instance;
        input::instance = nullptr;
    }
}

bool IsKeyDown(unsigned int key)
{
    return input::instance->currentKeyStates.at(key);
}

bool QueryKey(unsigned int key, bool& justPressed, bool& justReleased)
{
    bool pressed = input::instance->currentKeyStates.at(key);
    justPressed = pressed && !input::instance->previousKeyStates.at(key);
    justReleased = !pressed && input::instance->previousKeyStates.at(key);

    return pressed;
}

float2 GetCursorDelta()
{
    return input::instance->cursor - input::instance->lastCursor;
}

void UpdateInput(glfw::window* window)
{
    input::instance->previousKeyStates = input::instance->currentKeyStates;
    for (unsigned int key = 0; key < input::MaxKeys; ++key) {
        input::instance->currentKeyStates.at(key) = glfwGetKey(window->context, key);
    }

    input::instance->lastCursor = input::instance->cursor;
    glfw::GetMousePosition(window, input::instance->cursor.x, input::instance->cursor.y);

    if (input::instance->cursorMode == cursor_mode::locked) {
        input::instance->lastCursor = input::instance->cursor;

        int w, h;
        glfw::GetWindowSize(window, w, h);

        float hw = (float)w / 2.0f;
        float hh = (float)h / 2.0f;
        glfwSetCursorPos(window->context, (double)hw, (double)hh);

        input::instance->cursor.x = hw;
        input::instance->cursor.y = hh;
    }
}
