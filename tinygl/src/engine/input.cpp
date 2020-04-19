#include "engine/input.h"

#include "gl/window.h"
#include "glfw/glfw.h"

input_t* input_t::instance = nullptr;

void CreateInput()
{
    input_t::instance = new input_t;

    std::fill(input_t::instance->currentKeyStates.begin(), input_t::instance->currentKeyStates.end(), false);
    input_t::instance->previousKeyStates = input_t::instance->currentKeyStates;
}

void DestroyInput()
{
    if (input_t::instance)
    {
        delete input_t::instance;
        input_t::instance = nullptr;
    }
}

bool IsKeyDown(unsigned int key)
{
    input_t* input = input_t::instance;
    return input->currentKeyStates.at(key);
}

bool QueryKey(unsigned int key, bool& justPressed, bool& justReleased)
{
    input_t* input = input_t::instance;

    bool pressed = input->currentKeyStates.at(key);
    justPressed = pressed && !input->previousKeyStates.at(key);
    justReleased = !pressed && input->previousKeyStates.at(key);

    return pressed;
}

float2 GetCursorDelta()
{
    input_t* input = input_t::instance;
    return input->cursor - input->lastCursor;
}

void UpdateInput(glfw::window* window)
{
    input_t* input = input_t::instance;

    input->previousKeyStates = input->currentKeyStates;
    for (unsigned int key = 0; key < input_t::MaxKeys; ++key) {
        input->currentKeyStates.at(key) = glfwGetKey(window->context, key);
    }

    input->lastCursor = input->cursor;
    glfw::GetMousePosition(window, input->cursor.x, input->cursor.y);

    if (input->cursorMode == cursor_mode::locked) {
        input->lastCursor = input->cursor;

        int w, h;
        glfw::GetWindowSize(window, w, h);

        float hw = (float)w / 2.0f;
        float hh = (float)h / 2.0f;
        glfwSetCursorPos(window->context, (double)hw, (double)hh);

        input->cursor.x = hw;
        input->cursor.y = hh;
    }
}
