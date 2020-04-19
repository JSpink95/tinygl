#pragma once

namespace gl
{
    struct shader;
    struct texture;
    struct vertex_array;

    struct vertex_blob {
        void* data = nullptr;
        unsigned int size;
    };

    struct mesh {
        vertex_blob* vertices = nullptr;
        vertex_array* va = nullptr;
    };

    struct textured_mesh: public mesh {
        texture* image = nullptr;
    };

    mesh* CreateMesh(unsigned int numVertices, unsigned int stride, void* data);

    void DrawMesh(shader* s, mesh* m, unsigned int mode);
    void DrawMesh(shader* s, textured_mesh* tm, unsigned int mode);

    void DrawMeshInstanced(shader* s, mesh* m, unsigned int mode, unsigned int count);
    void DrawMeshInstanced(shader* s, textured_mesh* tm, unsigned int mode, unsigned int count);
}