
#pragma once

#include "gl/core.h"

namespace gl
{
    struct buffer;

    struct vertex_array
    {
        unsigned int indexCount;

        Id id;
        buffer* buffers[8];
        buffer* indices;
    };

    using vao = vertex_array;

    vertex_array* CreateVertexArray();
    void DestroyVertexArray(vertex_array* va);

    void BindVertexArray(vertex_array* va);
    void DrawVertexArray(vertex_array* va, unsigned int mode, unsigned int count);
    void DrawVertexArrayInstanced(vertex_array* va, unsigned int mode, unsigned int count, unsigned int instances);

    void SetIndexBuffer(vertex_array* va, buffer* indices, unsigned int count);
    void SetVertexBuffer(vertex_array* va, buffer* vb, unsigned int index);

    void EnableVertexAttribute(vertex_array* va, unsigned int buffer, unsigned int index, unsigned int size, unsigned int stride, unsigned int offset);
}
