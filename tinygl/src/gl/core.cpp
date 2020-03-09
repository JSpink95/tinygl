
#include "gl/core.h"
#include "gl/window.h"
#include "glfw/glfw.h"

namespace glfw
{
    bool initialise()
    {
        if (glfwInit() != GLFW_TRUE)
        {
            return false;
        }

        return true;
    }

    void terminate()
    {
        glfwTerminate();
    }
}

namespace glew
{
    bool initialise()
    {
        glewExperimental = true;
        if (glewInit() != GLEW_OK)
        {
            return false;
        }

        return true;
    }
}

namespace gl
{
    // buffer bits
    const unsigned int buffer_bits::color = GL_COLOR_BUFFER_BIT;
    const unsigned int buffer_bits::depth = GL_DEPTH_BUFFER_BIT;
    const unsigned int buffer_bits::stencil = GL_STENCIL_BUFFER_BIT;

    // gl features
    const unsigned int features::depth_test = GL_DEPTH_TEST;
    const unsigned int features::face_cull = GL_CULL_FACE;

    void SetClearColor(float r, float g, float b, float a)
    {
        glClearColor(r, g, b, a);
    }

    void Clear(unsigned int bits)
    {
        glClear(bits);
    }

    void Enable(unsigned int feature)
    {
        glEnable(feature);
    }

    void Disable(unsigned int feature)
    {
        glDisable(feature);
    }
}