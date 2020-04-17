
#pragma once

#include <functional>

struct GLFWwindow;

namespace glfw
{
    using WindowCloseCallback = std::function<void(GLFWwindow*)>;

    struct window
    {
        GLFWwindow *context = nullptr;

        WindowCloseCallback onWindowClose;
    };

    window* CreateWindow(const char* title, const int w, const int h);
    void DestroyWindow(window* w);

    void PollWindowEvents(window* w);
    void SwapBuffers(window* w);

    void SetTitle(window* w, const char* title);
    void SetSize(window* w, const int x, const int y);
    void SetPosition(window* w, const int x, const int y);

    void SetWindowCloseCallback(window* w, WindowCloseCallback callback);

    void GetWindowSize(window* w, int& width, int& height);
    void GetMousePosition(window* w, float& mx, float& my);

    void SetCursorVisible(window* w, bool visible);
}