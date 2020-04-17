
#include "engine/timer.h"

#include "gl/core.h"
#include "gl/window.h"
#include "gl/shader.h"
#include "gl/texture.h"
#include "gl/buffer.h"
#include "gl/vertex_array.h"
#include "gl/framebuffer.h"

#include "maths/maths.h"

#include "game/chunk.h"
#include "game/camera.h"

#include "glfw/glfw.h"

#include <glm/gtc/noise.hpp>

const double fps_60 = 1.0 / 60.0;

struct sdf_shape
{
    float4 data;
    float3 position;
    float type;
};

template<unsigned int size>
struct sdf_shape_buffer_t
{
    sdf_shape shapes[size];
};

template<unsigned int s>
struct chunk_t
{
    static constexpr const unsigned int width = s;
    static constexpr const unsigned int height = s;
    static constexpr const unsigned int depth = s;
    static constexpr const unsigned int numVoxels = s * s * s;

    float4 location;
    int voxels[numVoxels];
};

using chunk = chunk_t<32>;

struct chunk_buffer
{
    chunk chunks;
};

using sdf_shape_buffer = sdf_shape_buffer_t<32>;

const float PLANE_TYPE = 0.0f;
const float SPHERE_TYPE = 1.0f;
const float BOX_TYPE = 2.0f;

const float2 ScreenDimensions = float2(1280.0f, 720.0f);

int main()
{
    glfw::initialise();
    glfw::window* window = glfw::CreateWindow("TopDown", (int)ScreenDimensions.x, (int)ScreenDimensions.y);

    glew::initialise();

    bool exit = false;
    glfw::SetWindowCloseCallback(window, [&exit](GLFWwindow* w) -> void
    {
        exit = true;
    });

    gl::program* raymarchCompute = gl::CreateComputeProgramFromFile("shaders\\raymarch.ocs");
    gl::program* raymarchChunkCompute = gl::CreateComputeProgramFromFile("shaders\\raymarch-chunk.ocs");
    
    const unsigned int sdfShapeCount = 4;
    sdf_shape_buffer sdfShapes;
    sdfShapes.shapes[0] = { float4(0.0f, 1.0f, 0.0f,  1.0f), float3( 0.0f, 0.0f, 0.0f), PLANE_TYPE };
    sdfShapes.shapes[1] = { float4(1.0f, 0.0f, 0.0f,  0.0f), float3( 0.5f, 0.0f, 0.0f), SPHERE_TYPE };
    sdfShapes.shapes[2] = { float4(1.0f, 0.0f, 0.0f,  0.0f), float3(-0.5f, 0.0f, 0.0f), SPHERE_TYPE };
    sdfShapes.shapes[3] = { float4(1.0f, 0.5f, 1.0f,  0.0f), float3( 0.0f, 0.0f, 0.0f), BOX_TYPE };

    chunk_buffer chunks;
    for (unsigned int x = 0; x < chunk::width; ++x) {
        for (unsigned int y = 0; y < chunk::height; ++y) {
            for (unsigned int z = 0; z < chunk::depth; ++z) {
                unsigned int index = x * chunk::height * chunk::depth + y * chunk::depth + z;
                float p = math::perlin(float3((float)x * 0.01f, (float)z * 0.01f, 0.0f));
                chunks.chunks.voxels[index] = 1;// (p < (y / (float)chunk::height)) ? 1 : 0;
            }
        }
    }

    gl::buffer* sdfShapeBuffer = gl::CreateComputeBuffer(sizeof(sdf_shape_buffer), &sdfShapes, true);
    gl::buffer* chunkBuffer = gl::CreateComputeBuffer(sizeof(chunk_buffer), &chunks, true);

    gl::program* imageShader = gl::CreateShaderProgramFromFile("shaders\\render_image.vs", "shaders\\render_image.ps");
    gl::program* chunkShader = gl::CreateShaderProgramFromFile("shaders\\chunk.vs", "shaders\\chunk.ps");

    gf::camera* camera = gf::CreateCamera(70.0f, float3(1.0f, 4.0f, 6.0f), float3(0.0f));

    gl::texture* fbTexture = gl::CreateTexture2D((int)ScreenDimensions.x, (int)ScreenDimensions.y, GL_FLOAT, GL_RGBA32F, GL_RGBA, nullptr);
    gl::SetTexture2DParameter(fbTexture, gl::texture::parameter::min_filter, gl::texture::parameter::filter::point);
    gl::SetTexture2DParameter(fbTexture, gl::texture::parameter::max_filter, gl::texture::parameter::filter::point);

    const unsigned int screenVertexCount = 6;
    float2 screenVertices[screenVertexCount] = {
        float2(-1.0f, -1.0f), float2(1.0f, -1.0f), float2( 1.0f, 1.0f),
        float2(-1.0f, -1.0f), float2(1.0f,  1.0f), float2(-1.0f, 1.0f),
    };

    gl::buffer* screenVertexBuffer = gl::CreateVertexBuffer(sizeof(float2) * screenVertexCount, screenVertices);
    gl::vertex_array* screen = gl::CreateVertexArray();
    screen->vertexCount = screenVertexCount;
    gl::SetIndexBuffer(screen, nullptr, 0);
    gl::SetVertexBuffer(screen, screenVertexBuffer, 0);
    gl::EnableVertexAttribute(screen, 0, 0, 2, sizeof(float2), 0);

    eng::timer* frameTimer = eng::CreateTimer();

    glDisable(GL_DEPTH_TEST);

    float sphereX = 0.0f;

    float totalTime = 0.0f;
    while (!exit)
    {
        float dt = (float)eng::to_seconds(eng::time_mark(frameTimer));
        totalTime += dt;

        sdfShapes.shapes[3].position.x = glm::sin(totalTime) * 2.0f;
        gl::UpdateComputeBuffer(sdfShapeBuffer, sizeof(sdf_shape_buffer), &sdfShapes);

        //{
        //    gl::BindProgram(raymarchCompute);
        //    gl::BindComputeBuffer(sdfShapeBuffer, 1);
        //
        //    float4x4 viewMatrix = gf::getViewMatrix(camera);
        //    float4x4 projectionMatrix = gf::getProjectionMatrix(window, camera);
        //
        //    gl::UploadUniform(raymarchCompute, "u_ShapeCount", (int)sdfShapeCount);
        //    gl::UploadUniform(raymarchCompute, "u_Dimensions", ScreenDimensions);
        //    gl::UploadUniform(raymarchCompute, "u_CameraToWorld", math::inverse(viewMatrix));
        //    gl::UploadUniform(raymarchCompute, "u_CameraInverseProjection", math::inverse(projectionMatrix));
        //
        //    glBindImageTexture(0, fbTexture->id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
        //
        //    gl::DispatchComputeProgram(raymarchCompute, (int)ScreenDimensions.x / 32, (int)ScreenDimensions.y / 32, 1);
        //    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        //}

        {
            gl::BindProgram(raymarchChunkCompute);
            gl::BindComputeBuffer(chunkBuffer, 1);
        
            float4x4 viewMatrix = gf::getViewMatrix(camera);
            float4x4 projectionMatrix = gf::getProjectionMatrix(window, camera);
        
            gl::UploadUniform(raymarchChunkCompute, "u_ChunkCount", 1);
            gl::UploadUniform(raymarchChunkCompute, "u_Dimensions", ScreenDimensions);
            gl::UploadUniform(raymarchChunkCompute, "u_CameraToWorld", math::inverse(viewMatrix));
            gl::UploadUniform(raymarchChunkCompute, "u_CameraInverseProjection", math::inverse(projectionMatrix));
        
            glBindImageTexture(0, fbTexture->id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
        
            gl::DispatchComputeProgram(raymarchChunkCompute, (int)ScreenDimensions.x / 32, (int)ScreenDimensions.y / 32, 1);
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        }

        gl::SetClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        gl::Clear(gl::buffer_bits::color);

        gl::BindProgram(imageShader);
        gl::UploadUniform(imageShader, "u_Texture", fbTexture, 0);

        gl::BindVertexArray(screen);
        gl::DrawVertexArray(screen, GL_TRIANGLES, 0);

        glfw::PollWindowEvents(window);
        glfw::SwapBuffers(window);

        while (eng::to_seconds(eng::time_elapsed(frameTimer)) < fps_60);
    }

    gf::DestroyCamera(camera);

    glfw::DestroyWindow(window);
    glfw::terminate();
}