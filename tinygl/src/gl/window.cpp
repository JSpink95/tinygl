
#include "gl/window.h"
#include "glfw/glfw.h"

namespace glfw
{
    window* CreateWindow(const char* title, const int w, const int h)
    {
        window* result = new window;

        result->context = glfwCreateWindow(w, h, title, nullptr, nullptr);
        glfwMakeContextCurrent(result->context);
        glfwShowWindow(result->context);

        glfwSetWindowUserPointer(result->context, result);

        return result;
    }

    void DestroyWindow(window* w)
    {
        if (w && w->context)
        {
            glfwDestroyWindow(w->context);
            delete w;
        }
    }

    void PollWindowEvents(window* w)
    {
        glfwPollEvents();
    }

    void SwapBuffers(window* w)
    {
        glfwSwapBuffers(w->context);
    }

    void SetTitle(window* w, const char* title)
    {
        glfwSetWindowTitle(w->context, title);
    }

    void SetSize(window* w, const int x, const int y)
    {
        glfwSetWindowSize(w->context, x, y);
    }

    void SetPosition(window* w, const int x, const int y)
    {
        glfwSetWindowPos(w->context, x, y);
    }

    void SetWindowCloseCallback(window* w, WindowCloseCallback callback)
    {
        w->onWindowClose = callback;
        glfwSetWindowCloseCallback(w->context, [](GLFWwindow* w) -> void
        {
            window* userWindow = (window*)glfwGetWindowUserPointer(w);
            userWindow->onWindowClose(w);
        });
    }
    
    void GetWindowSize(window* w, int& width, int& height)
    {
        glfwGetWindowSize(w->context, &width, &height);
    }

    void GetMousePosition(window* w, float& mx, float& my)
    {
        double dmx, dmy;
        glfwGetCursorPos(w->context, &dmx, &dmy);

        mx = (float)dmx;
        my = (float)dmy;
    }

    void SetCursorVisible(window* w, bool visible)
    {
        glfwSetInputMode(w->context, GLFW_CURSOR, visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }
}
