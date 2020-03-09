
#pragma once

namespace glfw
{
    bool initialise();
    void terminate();
}

namespace glew
{
    bool initialise();
}

namespace gl
{
    using Id = unsigned int;

    struct buffer_bits
    {
        static const unsigned int color;
        static const unsigned int depth;
        static const unsigned int stencil;
    };

    struct features
    {
        static const unsigned int depth_test;
        static const unsigned int face_cull;
    };

    void SetClearColor(float r, float g, float b, float a);
    void Clear(unsigned int bits);
    void Enable(unsigned int feature);
    void Disable(unsigned int feature);
}