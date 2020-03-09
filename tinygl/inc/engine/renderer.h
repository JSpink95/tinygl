
#pragma once

namespace engine
{
    class renderer
    {
    public:
        struct quad
        {
            float x, y;
            float width, height;
        };

    public:
        static void BeginScene();
        static void EndScene();

        static void DrawQuad(const quad& q);
    };
}