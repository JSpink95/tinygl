// stl
//#include <ctime>

// engine core
#include "engine/timer.h"
#include "engine/input.h"
#include "engine/random.h"

#include "glfw/glfw.h"

#include "gl/core.h"
#include "gl/window.h"
#include "gl/buffer.h"
#include "gl/vertex_array.h"
#include "gl/shader.h"
#include "gl/texture.h"

// maths
#include "maths/maths.h"
#include "glm/gtc/noise.hpp"

// game
#include "game/chunk.h"
#include "game/chunk_manager.h"
#include "game/camera.h"
#include "game/player.h"
#include "game/grass.h"
#include "ai/mob.h"

#include <ctime>

struct position_vertex {
    float3 position;
};

gl::vertex_array* CreateChunkBounds(const chunk_config& config) {

    std::vector<unsigned int> indices;
    std::vector<position_vertex> vertices;

    const int gridSize = config.size;
    const float fGridSize = (float)gridSize * config.cellSize;

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

gl::vertex_array* CreateTexturedQuad(float size) {

    struct vertex {
        float3 position;
        float2 texcoord;
    };

    float hs = size / 2.0f;
    vertex vertices[4] = {
        { float3(-hs, 0.0f, 0.0f), float2(0.0f, 0.0f) },
        { float3( hs, 0.0f, 0.0f), float2(1.0f, 0.0f) },
        { float3( hs, size, 0.0f), float2(1.0f, 1.0f) },
        { float3(-hs, size, 0.0f), float2(0.0f, 1.0f) },
    };

    unsigned int indices[6] = {
        0, 2, 1,
        0, 3, 2,
    };

    gl::buffer* indexBuffer = gl::CreateIndexBuffer(sizeof(unsigned int) * 6, indices);
    gl::buffer* vertexBuffer = gl::CreateVertexBuffer(sizeof(vertex) * 4, vertices);

    gl::vertex_array* va = gl::CreateVertexArray();
    gl::SetIndexBuffer(va, indexBuffer, 6);
    gl::SetVertexBuffer(va, vertexBuffer, 0);

    gl::EnableVertexAttribute(va, 0, 0, 3, sizeof(vertex), offsetof(vertex, position));
    gl::EnableVertexAttribute(va, 0, 1, 2, sizeof(vertex), offsetof(vertex, texcoord));

    return va;
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

    eng::random::seed(std::time(0));
    CreateInput();

    input_t::instance->cursorMode = cursor_mode::locked;
    glfw::SetCursorVisible(window, false);

    perlin_config perlinConfig;
    //perlinConfig.seed = (int)std::time(0);
    perlinConfig.seed = 404;
    perlinConfig.scale = 0.02f;
    perlinConfig.amplitude = 12.0f;

    chunk_config config = chunk_config::defaultConfig;
    config.cellSize = 1.0f;
    chunk_manager* cm = tgl::CreateChunkManager(config, perlinConfig);

    gl::program* terrainShader = gl::CreateShaderProgramFromFile("shaders\\chunk.vs", "shaders\\chunk.gs", "shaders\\chunk.ps");
    gl::program* colorShader = gl::CreateShaderProgramFromFile("shaders\\color.vs", "shaders\\colors.ps");
    gl::program* textureShader = gl::CreateShaderProgramFromFile("shaders\\texture.vs", "shaders\\texture.ps");
    gl::program* grassShader = gl::CreateShaderProgramFromFile("shaders\\grass.vs", "shaders\\grass.ps");

    player_t* player = CreatePlayer();
    mob_t* mob = tg::CreateMob(cm, nullptr, float3(16.0f, 4.0f, 16.0f));

    gl::vertex_array* chunkBounds = CreateChunkBounds(config);
    gl::vertex_array* texturedQuad = CreateTexturedQuad(1.0f);

    mob->mesh = texturedQuad;

    gl::texture* grassTexture = gl::CreateTexture2D("textures\\grass.png");
    gl::SetTexture2DParameter(grassTexture, gl::texture::parameter::max_filter, gl::texture::parameter::filter::point);
    gl::SetTexture2DParameter(grassTexture, gl::texture::parameter::min_filter, gl::texture::parameter::filter::point);

    eng::timer* timer = eng::CreateTimer();
    float time = 0.0f;

    while (!exit)
    {
        float dt = (float)eng::to_seconds(eng::time_mark(timer));
        time += dt;

        UpdateInput(window);
        pHandleInput(player, cm, window, dt);
        tgl::UpdateChunks(cm, player->position, perlinConfig, dt);
        tg::UpdateMob(mob, dt);

        chunk_id cid = tgl::GetChunkIdFromWorldPosition(player->position);

        gl::SetClearColor(1.0f, 0.0f, 1.0f, 0.0f);
        gl::Clear(gl::buffer_bits::color | gl::buffer_bits::depth);

        glm::mat4 pr = cGetProjectionMatrix(player->camera, 1920.0f, 1080.0f);
        glm::mat4 vi = cGetViewMatrix(player->camera);

        gl::BindProgram(terrainShader);
        gl::UploadUniform(terrainShader, "u_vp", pr * vi);

        gl::BindProgram(grassShader);
        gl::UploadUniform(grassShader, "u_vp", pr * vi);
        gl::UploadUniform(grassShader, "u_time", time);
        gl::UploadUniform(grassShader, "u_texture", grassTexture, 0);

        gl::BindProgram(textureShader);
        gl::UploadUniform(textureShader, "u_vp", pr * vi);

        tgl::SubmitDrawbatch(cm, player->camera, terrainShader, grassShader);
        tg::SubmitDrawBatch(mob, player->camera, textureShader);

        glfw::PollWindowEvents(window);
        glfw::SwapBuffers(window);

        while (eng::to_seconds(eng::time_elapsed(timer)) < (1.0f / 60.0f));
    }

    DestroyPlayer(player);
    tgl::DestroyChunkManager(cm);
    DestroyInput();

    glfw::DestroyWindow(window);
    glfw::terminate();

    return 0;
}