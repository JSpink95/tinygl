
#include "gl/mesh.h"
#include "gl/vertex_array.h"

namespace gl
{
    mesh* CreateMesh(unsigned int numVertices, unsigned int stride, void* data)
    {
        mesh* result = new mesh;
        result->vertices = new vertex_blob{
            data, stride * numVertices
        };

        return result;
    }

    void DrawMesh(shader* s, mesh* m, unsigned int mode)
    {
        gl::BindVertexArray(m->va);
        gl::DrawVertexArray(m->va, mode, 0);
    }

    void DrawMesh(shader* s, textured_mesh* tm, unsigned int mode)
    {
        gl::BindVertexArray(tm->va);
        gl::DrawVertexArray(tm->va, mode, 0);
    }

    void DrawMeshInstanced(shader* s, mesh* m, unsigned int mode, unsigned int count)
    {
        gl::BindVertexArray(m->va);
        gl::DrawVertexArrayInstanced(m->va, mode, 0, count);
    }

    void DrawMeshInstanced(shader* s, textured_mesh* tm, unsigned int mode, unsigned int count)
    {
        gl::BindVertexArray(tm->va);
        gl::DrawVertexArrayInstanced(tm->va, mode, 0, count);
    }
}