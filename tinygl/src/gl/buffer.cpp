
#include "gl/buffer.h"
#include "glfw/glfw.h"

namespace gl
{
    // 
    // vertex buffer
    // 

    buffer* CreateVertexBuffer(unsigned int size)
    {
        buffer* result = new buffer;

        glGenBuffers(1, &result->id);
        glBindBuffer(GL_ARRAY_BUFFER, result->id);
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        return result;
    }

    buffer* CreateVertexBuffer(unsigned int size, const void* data)
    {
        buffer* result = new buffer;

        glGenBuffers(1, &result->id);
        glBindBuffer(GL_ARRAY_BUFFER, result->id);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        return result;
    }

    // 
    // index buffer
    // 

    buffer* CreateIndexBuffer(unsigned int size)
    {
        buffer* result = new buffer;

        glGenBuffers(1, &result->id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, result->id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        return result;
    }

    buffer* CreateIndexBuffer(unsigned int size, const void* data)
    {
        buffer* result = new buffer;

        glGenBuffers(1, &result->id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, result->id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        return result;
    }

    // 
    // instance buffer
    // 

    buffer* CreateInstanceBuffer(unsigned int size)
    {
        buffer* result = new buffer;

        glGenBuffers(1, &result->id);
        glBindBuffer(GL_ARRAY_BUFFER, result->id);
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        return result;
    }
    
    buffer* CreateInstanceBuffer(unsigned int size, const void* data)
    {
        buffer* result = new buffer;

        glGenBuffers(1, &result->id);
        glBindBuffer(GL_ARRAY_BUFFER, result->id);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        return result;
    }

    // 
    // uniform buffer
    // 

    buffer* CreateUniformBuffer(unsigned int size)
    {
        buffer* result = new buffer;

        glGenBuffers(1, &result->id);
        glBindBuffer(GL_UNIFORM_BUFFER, result->id);
        glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        return result;
    }

    buffer* CreateUniformBuffer(unsigned int size, const void* data)
    {
        buffer* result = new buffer;

        glGenBuffers(1, &result->id);
        glBindBuffer(GL_UNIFORM_BUFFER, result->id);
        glBufferData(GL_UNIFORM_BUFFER, size, data, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        return result;
    }
    
    void SetUniformBufferBinding(buffer* ub, unsigned int binding, unsigned int size)
    {
        glBindBufferRange(GL_UNIFORM_BUFFER, binding, ub->id, 0, size);
    }

    void SetUniformBufferData(buffer* ub, unsigned int size, const void* data)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, ub->id);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    // 
    // compute buffer
    // 

    buffer* CreateComputeBuffer(unsigned int size)
    {
        buffer* result = new buffer;

        glGenBuffers(1, &result->id);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, result->id);
        glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        return result;
    }

    buffer* CreateComputeBuffer(unsigned int size, const void* data, bool isDynamic)
    {
        buffer* result = new buffer;

        glGenBuffers(1, &result->id);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, result->id);
        glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        return result;
    }
    
    void UpdateComputeBuffer(buffer* buf, unsigned int size, const void* data)
    {
        if (buf != nullptr)
        {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, buf->id);
            glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }
    }

    void DestroyBuffer(buffer* buffer)
    {
        if (buffer != nullptr)
        {
            glDeleteBuffers(1, &buffer->id);
            delete buffer;
        }
    }

    void BindVertexBuffer(buffer* buf)
    {
        glBindBuffer(GL_ARRAY_BUFFER, buf ? buf->id : 0);
    }

    void BindIndexBuffer(buffer* buf)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf ? buf->id : 0);
    }

    void BindComputeBuffer(buffer* buf, unsigned int binding)
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buf->id);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, buf->id);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }
}