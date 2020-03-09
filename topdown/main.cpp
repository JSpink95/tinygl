
#include "engine/timer.h"

#include "gl/core.h"
#include "gl/window.h"

#include "maths/maths.h"

const double fps_60 = 1.0 / 60.0;

int main()
{
    glfw::initialise();
    glfw::window* window = glfw::CreateWindow("TopDown", 640, 480);

    glew::initialise();

    bool exit = false;
    glfw::SetWindowCloseCallback(window, [&exit](GLFWwindow* w) -> void
    {
        exit = true;
    });

    eng::timer* frameTimer = eng::CreateTimer();

    float totalTime = 0.0f;
    while (!exit)
    {
        float dt = (float)eng::to_seconds(eng::time_mark(frameTimer));
        totalTime += dt;

        gl::SetClearColor(math::sin(totalTime) * 0.5f + 0.5f, 0.0f, 0.0f, 0.0f);
        gl::Clear(gl::buffer_bits::color);

        glfw::PollWindowEvents(window);
        glfw::SwapBuffers(window);

        while (eng::to_seconds(eng::time_elapsed(frameTimer)) < fps_60);
    }

    glfw::DestroyWindow(window);
    glfw::terminate();
}