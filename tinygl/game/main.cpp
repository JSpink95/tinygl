//
//#include "gl/core.h"
//#include "gl/window.h"
//#include "gl/shader.h"
//#include "gl/vertex_array.h"
//#include "gl/buffer.h"
//#include "gl/texture.h"
//#include "gl/framebuffer.h"
//#include "maths/maths.h"
//
//#include "physics/simulator.h"
//#include "physics/intersection.h"
//
//#include "engine/random.h"
//
//#include "glfw/glfw.h"
//
//#include <chrono>
//#include <thread>
//
//void GenerateCubeSphere(unsigned int subdivisions, float radius, std::vector<float3>& outVertices, std::vector<unsigned int>& outIndices)
//{
//    static const float3 boxVertices[] = {
//        float3(-1.0f, -1.0f, -1.0f), // 0
//        float3( 1.0f, -1.0f, -1.0f), // 1
//        float3( 1.0f, -1.0f,  1.0f), // 2
//        float3(-1.0f, -1.0f,  1.0f), // 3
//
//        float3(-1.0f,  1.0f, -1.0f), // 4
//        float3( 1.0f,  1.0f, -1.0f), // 5
//        float3( 1.0f,  1.0f,  1.0f), // 6
//        float3(-1.0f,  1.0f,  1.0f), // 7
//    };
//
//    static const unsigned int boxIndices[] = {
//        0, 1, 2,
//        0, 2, 3,
//
//        4, 5, 6,
//        4, 6, 7,
//
//        0, 3, 7,
//        0, 7, 4,
//
//        1, 2, 6,
//        1, 6, 5,
//
//        3, 2, 6,
//        3, 6, 7,
//
//        0, 1, 5,
//        0, 5, 4,
//    };
//
//    std::vector<float3> vertices;
//    std::vector<unsigned int> indices;
//    for (unsigned int index = 0; index < 36; ++index)
//    {
//        vertices.push_back(boxVertices[boxIndices[index]]);
//        indices.push_back(boxIndices[index]);
//    }
//
//    for (unsigned int sd = 0; sd < subdivisions; ++sd)
//    {
//        std::vector<float3> subdivisionVertices = vertices;
//        std::vector<unsigned int> subdivisionIndices = indices;
//
//        vertices.clear();
//        indices.clear();
//
//        for (unsigned int index = 0; index < subdivisionIndices.size(); index += 3)
//        {
//            unsigned int ai = subdivisionIndices.at(index + 0);
//            unsigned int bi = subdivisionIndices.at(index + 1);
//            unsigned int ci = subdivisionIndices.at(index + 2);
//
//            const float3& a = subdivisionVertices.at(ai);
//            const float3& b = subdivisionVertices.at(bi);
//            const float3& c = subdivisionVertices.at(ci);
//
//            float3 ab = (a + b) / 2.0f;
//            float3 bc = (b + c) / 2.0f;
//            float3 ca = (c + a) / 2.0f;
//
//            unsigned int startIndex = vertices.size();
//            vertices.push_back(a);
//            vertices.push_back(ab);
//            vertices.push_back(b);
//            vertices.push_back(bc);
//            vertices.push_back(c);
//            vertices.push_back(ca);
//
//            indices.push_back(startIndex + 0); indices.push_back(startIndex + 1); indices.push_back(startIndex + 5);
//            indices.push_back(startIndex + 1); indices.push_back(startIndex + 3); indices.push_back(startIndex + 5);
//            indices.push_back(startIndex + 3); indices.push_back(startIndex + 4); indices.push_back(startIndex + 5);
//            indices.push_back(startIndex + 1); indices.push_back(startIndex + 2); indices.push_back(startIndex + 3);
//        }
//    }
//
//    outIndices = indices;
//    for (const float3& v : vertices)
//    {
//        outVertices.push_back(math::normalize(v) * radius);
//    }
//}
//
//struct level
//{
//    struct wall
//    {
//        float2 start, end;
//        float floorOffset, roofOffset;
//    };
//
//    unsigned int width, height;
//    std::vector<wall> wallList;
//
//    gl::program* floorShaderProgram;
//    gl::vertex_array* floor;
//
//    gl::program* wallShaderProgram;
//    gl::vertex_array* walls;
//};
//
//level* CreateLevel(unsigned int w, unsigned int h)
//{
//    struct floor_vertex
//    {
//        float3 position;
//        float2 texcoord;
//    };
//
//    level* lvl = new level;
//    lvl->width = w;
//    lvl->height = h;
//
//    std::vector<unsigned int> floorIndices;
//    std::vector<floor_vertex> floorVertices;
//
//    for (unsigned int x = 0; x < w; ++x)
//    {
//        for (unsigned int y = 0; y < h; ++y)
//        {
//            float3 a = float3((float)x, 0.0f, (float)y);
//            float3 b = a + float3(1.0f, 0.0f, 0.0f);
//            float3 c = a + float3(1.0f, 0.0f, 1.0f);
//            float3 d = a + float3(0.0f, 0.0f, 1.0f);
//
//            unsigned int startIndex = floorVertices.size();
//            floorVertices.push_back({ a, float2(a.x, a.z) });
//            floorVertices.push_back({ b, float2(b.x, b.z) });
//            floorVertices.push_back({ c, float2(c.x, c.z) });
//            floorVertices.push_back({ d, float2(d.x, d.z) });
//
//            floorIndices.push_back(startIndex + 0); floorIndices.push_back(startIndex + 1); floorIndices.push_back(startIndex + 2);
//            floorIndices.push_back(startIndex + 0); floorIndices.push_back(startIndex + 2); floorIndices.push_back(startIndex + 3);
//        }
//    }
//
//    gl::buffer* floorVertexBuffer = gl::CreateVertexBuffer(floorVertices.size() * sizeof(floor_vertex), &floorVertices.at(0));
//    gl::buffer* floorIndexBuffer = gl::CreateIndexBuffer(floorIndices.size() * sizeof(unsigned int), &floorIndices.at(0));
//
//    lvl->floor = gl::CreateVertexArray();
//    gl::SetIndexBuffer(lvl->floor, floorIndexBuffer);
//    gl::SetVertexBuffer(lvl->floor, floorVertexBuffer, 0);
//    gl::EnableVertexAttribute(lvl->floor, 0, 0, 3, sizeof(floor_vertex), 0);
//    gl::EnableVertexAttribute(lvl->floor, 0, 1, 2, sizeof(floor_vertex), 12);
//
//    return lvl;
//}
//
//void AddWallToLevel(level* lvl, const float2& start, const float2& end, float foff, float roff)
//{
//    level::wall wall;
//    wall.start = start;
//    wall.end = end;
//    wall.floorOffset = foff;
//    wall.roofOffset = roff;
//
//    lvl->wallList.push_back(wall);
//}
//
//void GenerateWallMesh(level* lvl)
//{
//    struct wall_vertex
//    {
//        float3 position;
//        float2 texcoord;
//    };
//
//    std::vector<unsigned int> indices;
//    std::vector<wall_vertex> vertices;
//
//    for (const level::wall& wall : lvl->wallList)
//    {
//        float3 a = float3(wall.start.x, wall.floorOffset, wall.start.y);
//        float3 b = float3(wall.end.x, wall.floorOffset, wall.start.y);
//        float3 c = float3(wall.end.x, wall.roofOffset, wall.end.y);
//        float3 d = float3(wall.start.x, wall.roofOffset, wall.end.y);
//
//        unsigned int startIndex = vertices.size();
//        vertices.push_back({ a, float2(0.0f, 0.0f) });
//        vertices.push_back({ b, float2(1.0f, 0.0f) });
//        vertices.push_back({ c, float2(1.0f, 1.0f) });
//        vertices.push_back({ d, float2(0.0f, 1.0f) });
//
//        indices.push_back(startIndex + 0); indices.push_back(startIndex + 1); indices.push_back(startIndex + 2);
//        indices.push_back(startIndex + 0); indices.push_back(startIndex + 2); indices.push_back(startIndex + 3);
//    }
//
//    gl::buffer* vertexBuffer = gl::CreateVertexBuffer(vertices.size() * sizeof(wall_vertex), &vertices.at(0));
//    gl::buffer* indexBuffer = gl::CreateIndexBuffer(indices.size() * sizeof(unsigned int), &indices.at(0));
//
//    lvl->walls = gl::CreateVertexArray();
//    gl::SetIndexBuffer(lvl->walls, indexBuffer);
//    gl::SetVertexBuffer(lvl->walls, vertexBuffer, 0);
//    gl::EnableVertexAttribute(lvl->walls, 0, 0, 3, sizeof(wall_vertex), 0);
//    gl::EnableVertexAttribute(lvl->walls, 0, 1, 2, sizeof(wall_vertex), 12);
//}
//
//unsigned int CalculateFloorIndexCount(level* lvl)
//{
//    // 3 indices per triangle, 2 triangles per cell
//    return lvl->width * lvl->height * 3 * 2;
//}
//
//unsigned int CalculateWallIndexCount(level* lvl)
//{
//    // 3 indices per triangle, 2 triangles per wall
//    return lvl->wallList.size() * 3 * 2;
//}
//
//int main(int argc, const char* argv)
//{
//    glfw::initialise();
//    glfw::window* window = glfw::CreateWindow("Compute Shader Test", 640, 480);
//
//    glew::initialise();
//
//    bool exit = false;
//    glfw::SetWindowCloseCallback(window, [&exit](GLFWwindow* w) -> void
//    {
//        exit = true;
//    });
//
//    struct vertex
//    {
//        float3 position;
//        float2 texcoord;
//    };
//
//    struct fullscreen_vertex
//    {
//        float2 position;
//    };
//
//    struct matrices
//    {
//        float4x4 view, projection;
//    };
//
//    //float4 matteColor = float4(0.4f, 0.85f, 0.35f, 1.0f);
//
//    vertex planeVertices[] = {
//        // floor
//        { float3(-1.0f, 0.0f, -1.0f), float2(0.0f, 0.0f) },
//        { float3( 1.0f, 0.0f, -1.0f), float2(1.0f, 0.0f) },
//        { float3( 1.0f, 0.0f,  1.0f), float2(1.0f, 1.0f) },
//        { float3(-1.0f, 0.0f,  1.0f), float2(0.0f, 1.0f) },
//    };
//
//    unsigned int planeIndices[] = {
//        0, 1, 2,
//        0, 2, 3,
//    };
//
//    static const vertex boxVertices[] = {
//        { float3(-1.0f, -1.0f, -1.0f), float2(0.0f, 0.0f) }, // 0
//        { float3( 1.0f, -1.0f, -1.0f), float2(1.0f, 0.0f) }, // 1
//        { float3( 1.0f, -1.0f,  1.0f), float2(1.0f, 1.0f) }, // 2
//        { float3(-1.0f, -1.0f,  1.0f), float2(0.0f, 1.0f) }, // 3
//
//        { float3(-1.0f,  1.0f, -1.0f), float2(0.0f, 0.0f) }, // 4
//        { float3( 1.0f,  1.0f, -1.0f), float2(1.0f, 0.0f) }, // 5
//        { float3( 1.0f,  1.0f,  1.0f), float2(1.0f, 1.0f) }, // 6
//        { float3(-1.0f,  1.0f,  1.0f), float2(0.0f, 1.0f) }, // 7
//    };
//
//    static const unsigned int boxIndices[] = {
//        0, 1, 2,
//        0, 2, 3,
//
//        4, 5, 6,
//        4, 6, 7,
//
//        0, 3, 7,
//        0, 7, 4,
//
//        1, 2, 6,
//        1, 6, 5,
//
//        3, 2, 6,
//        3, 6, 7,
//
//        0, 1, 5,
//        0, 5, 4,
//    };
//
//    fullscreen_vertex screenQuadVertices[] = {
//        { float2(-1.0f, -1.0f) },
//        { float2( 1.0f, -1.0f) },
//        { float2( 1.0f,  1.0f) },
//        { float2(-1.0f,  1.0f) },
//    };
//
//    unsigned int screenQuadIndices[] = {
//        0, 1, 2,
//        0, 2, 3,
//    };
//
//    vertex grassVertices[] = {
//        { float3(-0.5f, 0.0f, -0.5f), float2(0.0f, 0.0f) },
//        { float3( 0.5f, 0.0f,  0.5f), float2(1.0f, 0.0f) },
//        { float3( 0.5f, 1.0f,  0.5f), float2(1.0f, 1.0f) },
//        { float3(-0.5f, 1.0f, -0.5f), float2(0.0f, 1.0f) },
//
//        { float3(-0.5f, 0.0f,  0.5f), float2(0.0f, 0.0f) },
//        { float3( 0.5f, 0.0f, -0.5f), float2(1.0f, 0.0f) },
//        { float3( 0.5f, 1.0f, -0.5f), float2(1.0f, 1.0f) },
//        { float3(-0.5f, 1.0f,  0.5f), float2(0.0f, 1.0f) },
//    };
//
//    unsigned int grassIndices[] = {
//        0, 1, 2,
//        0, 2, 3,
//
//        4, 5, 6,
//        4, 6, 7,
//    };
//
//    glEnable(GL_DEPTH_TEST);
//
//    gl::buffer* planeVertexBuffer = gl::CreateVertexBuffer(4 * sizeof(vertex), planeVertices);
//    gl::buffer* planeIndexBuffer = gl::CreateIndexBuffer(6 * sizeof(unsigned int), planeIndices);
//
//    gl::vertex_array* plane = gl::CreateVertexArray();
//    gl::SetIndexBuffer(plane, planeIndexBuffer);
//    gl::SetVertexBuffer(plane, planeVertexBuffer, 0);
//    gl::EnableVertexAttribute(plane, 0, 0, 3, sizeof(vertex), offsetof(vertex, position));
//    gl::EnableVertexAttribute(plane, 0, 1, 2, sizeof(vertex), offsetof(vertex, texcoord));
//
//    gl::buffer* cubeVertexBuffer = gl::CreateVertexBuffer(24 * sizeof(vertex), boxVertices);
//    gl::buffer* cubeIndexBuffer = gl::CreateIndexBuffer(36 * sizeof(unsigned int), boxIndices);
//
//    gl::vertex_array* cube = gl::CreateVertexArray();
//    gl::SetIndexBuffer(cube, cubeIndexBuffer);
//    gl::SetVertexBuffer(cube, cubeVertexBuffer, 0);
//    gl::EnableVertexAttribute(cube, 0, 0, 3, sizeof(vertex), offsetof(vertex, position));
//    gl::EnableVertexAttribute(cube, 0, 1, 2, sizeof(vertex), offsetof(vertex, texcoord));
//
//    gl::buffer* screenQuadBuffer = gl::CreateVertexBuffer(4 * sizeof(fullscreen_vertex), screenQuadVertices);
//    gl::buffer* screenQuadIndexBuffer = gl::CreateIndexBuffer(6 * sizeof(unsigned int), screenQuadIndices);
//
//    gl::vertex_array* screenquad = gl::CreateVertexArray();
//    gl::SetIndexBuffer(screenquad, screenQuadIndexBuffer);
//    gl::SetVertexBuffer(screenquad, screenQuadBuffer, 0);
//    gl::EnableVertexAttribute(screenquad, 0, 0, 2, sizeof(fullscreen_vertex), 0);
//
//    gl::buffer* grassVertexBuffer = gl::CreateVertexBuffer(8 * sizeof(vertex), grassVertices);
//    gl::buffer* grassIndexBuffer = gl::CreateIndexBuffer(12 * sizeof(unsigned int), grassIndices);
//
//    gl::vertex_array* grass = gl::CreateVertexArray();
//    gl::SetIndexBuffer(grass, grassIndexBuffer);
//    gl::SetVertexBuffer(grass, grassVertexBuffer, 0);
//    gl::EnableVertexAttribute(grass, 0, 0, 3, sizeof(vertex), 0);
//    gl::EnableVertexAttribute(grass, 0, 1, 2, sizeof(vertex), 12);
//
//    gl::program* standardUnlitprogram = gl::CreateShaderProgramFromFile("shaders\\standard_unlit.vs", "shaders\\standard_unlit.ps");
//    gl::program* standardUnlitInstancedProgram = gl::CreateShaderProgramFromFile("shaders\\standard_unlit_instanced.vs", "shaders\\standard_unlit_instanced.ps");
//    gl::program* renderImageProgram = gl::CreateShaderProgramFromFile("shaders\\render_image.vs", "shaders\\render_image.ps");
//
//    gl::program* computeProgram = gl::CreateComputeProgramFromFile("shaders\\raymarch.compute");
//
//    gl::framebuffer::attachment attachments[] = { { GL_RGBA, GL_UNSIGNED_BYTE } };
//    gl::framebuffer* framebuffer = gl::CreateFramebuffer(640, 480, 1, attachments, true);
//
//    unsigned char offWhiteTexturePixels[4 * 4 * 3] =
//    {
//        0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc,
//        0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc,
//        0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc,
//        0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc,
//    };
//
//    gl::texture* offWhiteTexture = gl::CreateTexture2D(4, 4, GL_UNSIGNED_BYTE, GL_RGB, GL_RGB, offWhiteTexturePixels);
//    gl::SetTexture2DParameter(offWhiteTexture, gl::texture::parameter::min_filter, gl::texture::parameter::filter::point);
//    gl::SetTexture2DParameter(offWhiteTexture, gl::texture::parameter::max_filter, gl::texture::parameter::filter::point);
//
//    unsigned char devTexturePixels[4 * 4 * 3] =
//    {
//        0xaa, 0xaa, 0xaa, 0xff, 0xff, 0xff, 0xaa, 0xaa, 0xaa, 0xff, 0xff, 0xff,
//        0xff, 0xff, 0xff, 0xaa, 0xaa, 0xaa, 0xff, 0xff, 0xff, 0xaa, 0xaa, 0xaa,
//        0xaa, 0xaa, 0xaa, 0xff, 0xff, 0xff, 0xaa, 0xaa, 0xaa, 0xff, 0xff, 0xff,
//        0xff, 0xff, 0xff, 0xaa, 0xaa, 0xaa, 0xff, 0xff, 0xff, 0xaa, 0xaa, 0xaa,
//    };
//
//    gl::texture* devTexture = gl::CreateTexture2D(4, 4, GL_UNSIGNED_BYTE, GL_RGB, GL_RGB, devTexturePixels);
//    gl::SetTexture2DParameter(devTexture, gl::texture::parameter::min_filter, gl::texture::parameter::filter::point);
//    gl::SetTexture2DParameter(devTexture, gl::texture::parameter::max_filter, gl::texture::parameter::filter::point);
//
//    // 0x66, 0xd9, 0x59
//    unsigned char grassTexturePixels[2 * 2 * 3] =
//    {
//        0x66, 0xd9, 0x59, 0x66, 0xd9, 0x59,
//        0x66, 0xd9, 0x59, 0x66, 0xd9, 0x59,
//    };
//
//    gl::texture* grassTexture = gl::CreateTexture2D(2, 2, GL_UNSIGNED_BYTE, GL_RGB, GL_RGB, grassTexturePixels);
//    gl::SetTexture2DParameter(grassTexture, gl::texture::parameter::min_filter, gl::texture::parameter::filter::point);
//    gl::SetTexture2DParameter(grassTexture, gl::texture::parameter::max_filter, gl::texture::parameter::filter::point);
//
//    gl::texture* screenTexture = gl::CreateTexture2D(640, 480, GL_FLOAT, GL_RGBA32F, GL_RGBA, nullptr);
//
//    matrices viewproj = {
//        math::lookAt(
//            float3(6.0f, 6.0f, 6.0f), // eye
//            float3(0.0f, 0.0f, 0.0f), // target
//            float3(0.0f, 1.0f, 0.0f)  // up
//        ),
//
//        math::perspectiveFov(
//            math::radians(60.0f),   // fov
//            640.0f,                 // width
//            480.0f,                 // height
//            0.01f,                  // near
//            100.0f                  // far
//        )
//    };
//
//    gl::buffer* matricesBuffer = gl::CreateUniformBuffer(sizeof(matrices), (const void*)&viewproj);
//
//    unsigned int grassTransformCount = 32;
//    std::vector<float4x4> grassTransforms;
//    for (unsigned int index = 0; index < grassTransformCount; ++index)
//    {
//        float3 location = {
//            engine::random::next_float() * 10.0f - 5.0f,
//            0.0f,
//            engine::random::next_float() * 10.0f - 5.0f
//        };
//
//        grassTransforms.push_back(math::translate(float4x4(1.0f), location));
//    }
//
//    level* lvl = CreateLevel(10, 5);
//    lvl->floorShaderProgram = standardUnlitprogram;
//
//    AddWallToLevel(lvl, float2(0.0f, 0.0f), float2(10.0f, 0.0f), 0.0f, 0.5f);
//    AddWallToLevel(lvl, float2(0.0f, 0.0f), float2(10.0f, 0.0f), 1.0f, 1.5f);
//    GenerateWallMesh(lvl);
//
//    physics::state po;
//    po.p = float3(0.0f, 5.0f, 0.0f);
//
//    double t = 0.0;
//    float dt = 1.0f / 30.0f;
//
//    while (!exit)
//    {
//        physics::integrate(po, t, dt);
//        
//        gl::BindProgram(computeProgram);
//        gl::UploadUniform(computeProgram, "outputTexture", screenTexture, 0);
//        glBindImageTexture(0, screenTexture->id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
//
//        gl::DispatchComputeProgram(computeProgram, 640 / 16, 480 / 16, 1);
//        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
//
//        // draw screen quad
//        gl::SetClearColor(1.0f, 0.0f, 1.0f, 0.0f);
//        gl::Clear(gl::buffer_bits::color | gl::buffer_bits::depth);
//
//        gl::BindProgram(renderImageProgram);
//        gl::UploadUniform(renderImageProgram, "u_Texture", screenTexture, 0);
//        //gl::UploadUniform(renderImageProgram, "u_Texture", texture, 0);
//
//        gl::BindVertexArray(screenquad);
//        gl::DrawVertexArray(screenquad, GL_TRIANGLES, 6);
//
//        glfw::PollWindowEvents(window);
//        glfw::SwapBuffers(window);
//
//        std::this_thread::sleep_for(std::chrono::milliseconds(16));
//    }
//    
//    gl::DestroyProgram(standardUnlitprogram);
//    gl::DestroyProgram(renderImageProgram);
//
//    glfw::DestroyWindow(window);
//    glfw::terminate();
//
//    return 0;
//}