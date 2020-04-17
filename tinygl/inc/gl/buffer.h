
#pragma once

#include "gl/core.h"

namespace gl
{
    struct buffer
    {
        Id id;
    };

    using vertex_buffer = buffer;
    using vbo = vertex_buffer;

    using index_buffer = buffer;
    using ibo = index_buffer;

    using uniform_buffer = buffer;
    using ubo = uniform_buffer;

    using compute_buffer = buffer;
    using cbo = compute_buffer;

    // vertex buffer

    buffer* CreateVertexBuffer(unsigned int size);
    buffer* CreateVertexBuffer(unsigned int size, const void* data);

    // index buffer
    buffer* CreateIndexBuffer(unsigned int size);
    buffer* CreateIndexBuffer(unsigned int size, const void* data);

    // uniform buffer
    buffer* CreateUniformBuffer(unsigned int size);
    buffer* CreateUniformBuffer(unsigned int size, const void* data);
    void SetUniformBufferBinding(buffer* ub, unsigned int binding, unsigned int size);
    void SetUniformBufferData(buffer* ub, unsigned int size, const void* data);

    // compute buffer
    buffer* CreateComputeBuffer(unsigned int size);
    buffer* CreateComputeBuffer(unsigned int size, const void* data, bool isDynamic);
    void UpdateComputeBuffer(buffer* buf, unsigned int size, const void* data);

    void DestroyBuffer(buffer* buf);

    void BindVertexBuffer(buffer* buf);
    void BindIndexBuffer(buffer* buf);
    void BindComputeBuffer(buffer* buf, unsigned int binding);
}
