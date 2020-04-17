
#include "gl/vertex_array.h"
#include "gl/buffer.h"
#include "glfw/glfw.h"

namespace gl
{
    vertex_array* CreateVertexArray()
    {
        vertex_array* result = new vertex_array;
        glCreateVertexArrays(1, &result->id);

        return result;
    }
    
    void DestroyVertexArray(vertex_array* va)
    {
        if (va != nullptr)
        {
            glDeleteVertexArrays(1, &va->id);
            delete va;
        }
    }
    
    void BindVertexArray(vertex_array* va)
    {
        glBindVertexArray(va ? va->id : 0);
        BindIndexBuffer(va ? va->indices : nullptr);
    }
    
    void DrawVertexArray(vertex_array* va, unsigned int mode, unsigned int count)
    {
        if (va->indices)
        {
            glDrawElements(mode, va->indexCount, GL_UNSIGNED_INT, (void*)0);
        }
        else
        {
            glDrawArrays(mode, 0, va->vertexCount);
        }
    }

    void DrawVertexArrayInstanced(vertex_array* va, unsigned int mode, unsigned int count, unsigned int instances)
    {
        if (va->indices)
        {
            glDrawElementsInstancedBaseVertex(mode, count, GL_UNSIGNED_INT, (void*)0, instances, 0);
        }
        else
        {
            glDrawArraysInstanced(mode, 0, va->vertexCount, instances);
        }
    }

    void SetIndexBuffer(vertex_array* va, buffer* indices, unsigned int count)
    {
        va->indexCount = count;
        va->indices = indices;
    }

    void SetVertexBuffer(vertex_array* va, buffer* vb, unsigned int index)
    {
        va->buffers[index] = vb;
    }

    void EnableVertexAttribute(vertex_array* va, unsigned int buffer, unsigned int index, unsigned int size, unsigned int stride, unsigned int offset)
    {
        glBindVertexArray(va->id);
        glBindBuffer(GL_ARRAY_BUFFER, va->buffers[buffer]->id);

        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, (const void*)offset);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
}
