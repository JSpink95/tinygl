// stl
//#include <ctime>

// engine core
#include "engine/timer.h"
#include "engine/input.h"
#include "gl/core.h"
#include "glfw/glfw.h"
#include "gl/window.h"
#include "gl/buffer.h"
#include "gl/vertex_array.h"
#include "gl/shader.h"

// maths
#include "maths/maths.h"
#include "glm/gtc/noise.hpp"

// game
#include "game/terrain.h"
#include "game/camera.h"

// shaders
const char* vsChunk = R"(
#version 440 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec4 a_color;

out vec3 gs_pos;
out vec4 gs_col;

void main()
{
    gs_pos = a_position;
    gs_col = a_color;
}

)";

const char* gsChunk = R"(
#version 440 core

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

in vec3 gs_pos[];
in vec4 gs_col[];

uniform mat4 u_w;
uniform mat4 u_vp;

out vec3 fs_normal;
out vec4 fs_position;
out vec4 fs_albedo;

void main()
{
    fs_normal = -normalize(cross(normalize(gs_pos[2] - gs_pos[0]), normalize(gs_pos[1] - gs_pos[0])));
    
    mat4 wvp = u_vp * u_w;
    fs_position = u_w * vec4(gs_pos[0], 1.0f);
    fs_albedo = gs_col[0];
    gl_Position = wvp * vec4(gs_pos[0], 1.0f);
    EmitVertex();

    fs_position = u_w * vec4(gs_pos[1], 1.0f);
    fs_albedo = gs_col[1];
    gl_Position = wvp * vec4(gs_pos[1], 1.0f);
    EmitVertex();

    fs_position = u_w * vec4(gs_pos[2], 1.0f);
    fs_albedo = gs_col[2];
    gl_Position = wvp * vec4(gs_pos[2], 1.0f);
    EmitVertex();

    EndPrimitive();
}

)";

const char* psChunk = R"(
#version 440 core

in vec3 fs_normal;
in vec4 fs_position;
in vec4 fs_albedo;

layout(location = 0) out vec4 o_color;

void main()
{
    float nu = dot(vec3(0.0f, 1.0f, 0.0f), fs_normal);

    vec3 l = normalize(vec3(10.0f, 34.0f, 16.0f) - fs_position.xyz);
    vec3 ambient = fs_albedo.rgb * 0.2f;
    vec3 albedo = vec3(0.1f);//fs_albedo.rgb;
    float nl = max(dot(fs_normal, l), 0.0f);

    o_color = vec4(ambient + albedo * nl, 1.0f);
}

)";

struct position_vertex {
    float3 position;
};

const char* vsColor = R"(
#version 440 core

layout(location = 0) in vec3 a_position;

uniform mat4 u_vp;
uniform mat4 u_w;

void main()
{
    gl_Position = u_vp * u_w * vec4(a_position, 1.0f);
}

)";

const char* psColor = R"(
#version 440 core

uniform vec3 u_color;

layout (location = 0) out vec4 o_color;

void main()
{
    o_color = vec4(u_color, 1.0f);
}

)";

struct triangle_vertex {
    float3 position;
    float index;
};

const char* vsTriangle = R"(
#version 440 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in float a_index;

uniform mat4 u_vp;
uniform mat4 u_w[3];

void main() {
    gl_Position = u_vp * u_w[int(a_index)] * vec4(a_position, 1.0f);
}

)";

const char* psTriangle = R"(
#version 440 core

uniform vec3 u_color;

layout(location = 0) out vec4 o_color;

void main() {
    o_color = vec4(u_color, 1.0f);
}


)";

gl::vertex_array* CreateFlatQuad() {

    position_vertex vertices[4] = {
        { float3(0.0f, 0.0f, 0.0f) },
        { float3(1.0f, 0.0f, 0.0f) },
        { float3(1.0f, 0.0f, 1.0f) },
        { float3(0.0f, 0.0f, 1.0f) },
    };

    unsigned int indices[6] = {
        0, 1, 2,
        2, 3, 0,
    };

    gl::buffer* vertexBuffer = gl::CreateVertexBuffer(sizeof(position_vertex) * 4, vertices);
    gl::buffer* indexBuffer = gl::CreateIndexBuffer(sizeof(unsigned int) * 6, indices);

    gl::vertex_array* mesh = gl::CreateVertexArray();
    gl::SetIndexBuffer(mesh, indexBuffer, 6);
    gl::SetVertexBuffer(mesh, vertexBuffer, 0);

    gl::EnableVertexAttribute(mesh, 0, 0, 3, sizeof(position_vertex), offsetof(position_vertex, position));

    return mesh;
}

gl::vertex_array* CreateChunkBounds(const terrain_chunk_config& config) {

    std::vector<unsigned int> indices;
    std::vector<position_vertex> vertices;

    const int gridSize = config.gridSize;
    const float fGridSize = (float)gridSize;

    for (int v = 0; v <= gridSize; ++v) {
        // x axis
        vertices.push_back({ float3((float)v,  0.0f,  0.0f) });
        vertices.push_back({ float3((float)v, fGridSize,  0.0f) });
        vertices.push_back({ float3((float)v,  0.0f, fGridSize) });
        vertices.push_back({ float3((float)v, fGridSize, fGridSize) });
        // z axis
        vertices.push_back({ float3( 0.0f,  0.0f, (float)v) });
        vertices.push_back({ float3( 0.0f, fGridSize, (float)v) });
        vertices.push_back({ float3(fGridSize,  0.0f, (float)v) });
        vertices.push_back({ float3(fGridSize, fGridSize, (float)v) });
        // y axis
        vertices.push_back({ float3( 0.0f, (float)v,  0.0f) });
        vertices.push_back({ float3(fGridSize, (float)v,  0.0f) });
        vertices.push_back({ float3( 0.0f, (float)v, fGridSize) });
        vertices.push_back({ float3(fGridSize, (float)v, fGridSize) });
        vertices.push_back({ float3( 0.0f, (float)v,  0.0f) });
        vertices.push_back({ float3( 0.0f, (float)v, fGridSize) });
        vertices.push_back({ float3(fGridSize, (float)v,  0.0f) });
        vertices.push_back({ float3(fGridSize, (float)v, fGridSize) });
    }

    gl::buffer* vertexBuffer = gl::CreateVertexBuffer(sizeof(position_vertex) * vertices.size(), &vertices.at(0));
    gl::vertex_array* mesh = gl::CreateVertexArray();
    mesh->vertexCount = vertices.size();

    gl::SetIndexBuffer(mesh, nullptr, 0);
    gl::SetVertexBuffer(mesh, vertexBuffer, 0);

    gl::EnableVertexAttribute(mesh, 0, 0, 3, sizeof(position_vertex), offsetof(position_vertex, position));

    return mesh;
}

gl::vertex_array* CreatePointList(unsigned int numPoints) {
    std::vector<triangle_vertex> vertices;
    for (unsigned int a = 0; a < numPoints; ++a) {
        vertices.push_back({ float3(0.0f), (float)a });
    }

    gl::buffer* vertexBuffer = gl::CreateVertexBuffer(sizeof(triangle_vertex) * vertices.size(), &vertices.at(0));
    gl::vertex_array* mesh = gl::CreateVertexArray();
    mesh->vertexCount = numPoints;

    gl::SetIndexBuffer(mesh, nullptr, 0);
    gl::SetVertexBuffer(mesh, vertexBuffer, 0);

    gl::EnableVertexAttribute(mesh, 0, 0, 3, sizeof(triangle_vertex), offsetof(triangle_vertex, position));
    gl::EnableVertexAttribute(mesh, 0, 1, 1, sizeof(triangle_vertex), offsetof(triangle_vertex, index));

    return mesh;
}

int main()
{
    glfw::initialise();
    glfw::window* window = glfw::CreateWindow("mmo", 1920, 1080);
    glew::initialise();

    bool exit = false;
    glfw::SetWindowCloseCallback(window, [&exit](GLFWwindow* window) -> void
    {
        exit = true;
    });

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    CreateInput();

    perlin_config perlinConfig;
    //perlinConfig.seed = (int)std::time(0);
    perlinConfig.seed = 404;
    perlinConfig.scale = 0.03f;
    perlinConfig.amplitude = 8.0f;

    terrain_chunk_config config = terrain_chunk_config::defaultConfig;
    config.gridSize = 32;

    terrain_chunk_manager* tcm = CreateTerrainChunkManager(config, perlinConfig);

    gl::program* terrainShader = gl::CreateShaderProgramFromSource(vsChunk, gsChunk, psChunk);
    gl::program* lineShader = gl::CreateShaderProgramFromSource(vsColor, psColor);
    gl::program* triangleShader = gl::CreateShaderProgramFromSource(vsTriangle, psTriangle);

    camera* cam = new camera;
    cam->fov = 70.0f;
    cam->eye = float3(0.0f, 8.2f, 0.0f);

    gl::vertex_array* chunkBounds = CreateChunkBounds(config);
    gl::vertex_array* flatQuad = CreateFlatQuad();
    gl::vertex_array* triangle = CreatePointList(3);
    gl::vertex_array* point = CreatePointList(1);

    eng::timer* timer = eng::CreateTimer();

    while (!exit)
    {
        float dt = (float)eng::to_seconds(eng::time_mark(timer));

        UpdateInput(window);
        cHandleInput(cam, window, dt);
        tcmUpdateChunks(tcm, cam->eye, perlinConfig, dt);

        float camHeight = 1.0f;

        float3 a, b, c;
        cam->eye.y = tcmGetBarycentricHeightAtWorldPosition(tcm, cam->eye, a, b, c) + camHeight;

        chunk_id cid = GetChunkIdFromWorldPosition(cam->eye);

        gl::SetClearColor(1.0f, 0.0f, 1.0f, 0.0f);
        gl::Clear(gl::buffer_bits::color | gl::buffer_bits::depth);

        glm::mat4 p = cGetProjectionMatrix(cam, 1920.0f, 1080.0f);
        glm::mat4 v = cGetViewMatrix(cam);

        gl::BindProgram(terrainShader);
        gl::UploadUniform(terrainShader, "u_vp", p * v);

        tcmSubmitDrawbatch(tcm, cam, terrainShader);

        gl::Clear(gl::buffer_bits::depth);


        glDisable(GL_CULL_FACE);

        // draw the triangle we are on
        {
            gl::BindProgram(triangleShader);
            gl::UploadUniform(triangleShader, "u_color", float3(1.0f, 0.0f, 0.0f));
            gl::UploadUniform(triangleShader, "u_vp", p * v);

            glm::mat4 tpos[3] = {
                glm::translate(glm::mat4(1.0f), a),
                glm::translate(glm::mat4(1.0f), c),
                glm::translate(glm::mat4(1.0f), b),
            };

            gl::UploadUniformArray(triangleShader, "u_w", 3, tpos);

            gl::BindVertexArray(triangle);
            gl::DrawVertexArray(triangle, GL_TRIANGLES, 3);
        }

        // draw the point we're on
        {
            glPointSize(4.0f);
            gl::Clear(gl::buffer_bits::depth);

            glm::mat4 ppos[1] = {
                glm::translate(glm::mat4(1.0f), cam->eye - float3(0.0f, camHeight, 0.0f))
            };

            gl::UploadUniform(triangleShader, "u_color", float3(1.0f, 1.0f, 0.0f));
            gl::UploadUniformArray(triangleShader, "u_w", 1, ppos);

            gl::BindVertexArray(point);
            gl::DrawVertexArray(point, GL_POINTS, 1);

        }

        glEnable(GL_CULL_FACE);

#if 0
        gl::BindProgram(lineShader);
        gl::UploadUniform(lineShader, "u_vp", p * v);
        gl::UploadUniform(lineShader, "u_w", glm::translate(glm::mat4(1.0f), float3(cid.x * config.gridSize, 0.0f, cid.y * config.gridSize)));
        gl::UploadUniform(lineShader, "u_color", float3(1.0f, 1.0f, 0.2f));

        gl::BindVertexArray(chunkBounds);
        gl::DrawVertexArray(chunkBounds, GL_LINES, chunkBounds->vertexCount);
#endif
        glfw::PollWindowEvents(window);
        glfw::SwapBuffers(window);

        while (eng::to_seconds(eng::time_elapsed(timer)) < (1.0f / 60.0f));
    }

    delete cam;
    DestroyTerrainChunkManager(tcm);
    DestroyInput();

    glfw::DestroyWindow(window);
    glfw::terminate();

    return 0;
}