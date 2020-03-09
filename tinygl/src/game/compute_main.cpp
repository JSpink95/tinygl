//
//#include "gl/core.h"
//#include "gl/window.h"
//
//#include "gl/buffer.h"
//#include "gl/shader.h"
//
//#include "gl/vertex_array.h"
//
//#include "glfw/glfw.h"
//
//#include "engine/timer.h"
//
//#include <chrono>
//#include <thread>
//#include <random>
//#include <ctime>
//#include <string>
//
//constexpr unsigned int CalculateNextPowerOfTwo(unsigned int v)
//{
//    v--;
//    v |= v >> 1;
//    v |= v >> 2;
//    v |= v >> 4;
//    v |= v >> 8;
//    v |= v >> 16;
//    v++;
//
//    return v;
//}
//
//struct particle
//{
//    float3 position;
//    float rotation;
//    float4 velocity;
//};
//
//template<unsigned int size>
//struct particle_buffer_t
//{
//    static constexpr const unsigned int element_count = size;
//    particle particles[size];
//};
//
//template<unsigned int dimensions>
//struct heightfield_t
//{
//    static constexpr const unsigned int width = dimensions;
//    static constexpr const unsigned int height = dimensions;
//    static constexpr const unsigned int size = width * height;
//
//    float4 heights[size];
//};
//
//namespace glm
//{
//    bool intersectRayWithPlane(
//        const vec3& p, const vec3& v,  // ray
//        const vec3& n, float d,  // plane
//        vec3& hit
//    ) {
//        float denom = dot(n, v);
//
//        // Prevent divide by zero:
//        if (abs(denom) <= 1e-4f)
//            return false;
//
//        // If you want to ensure the ray reflects off only
//        // the "top" half of the plane, use this instead:
//        if (-denom <= 1e-4f)
//            return false;
//
//        float t = -(dot(n, p) + d) / dot(n, v);
//
//        // Use pointy end of the ray.
//        // It is technically correct to compare t < 0,
//        // but that may be undesirable in a raytracer.
//        if (t <= 1e-4)
//            return false;
//
//        hit = p + t * v;
//        return true;
//    }
//}
//
//namespace gf
//{
//    struct ray
//    {
//        float3 origin;
//        float3 direction;
//    };
//
//    struct camera
//    {
//        float fov;
//        float3 eye;
//        float3 target;
//    };
//
//    camera* CreateCamera(float fov, const float3& eye, const float3& target)
//    {
//        camera* result = new camera;
//
//        result->fov = fov;
//        result->eye = eye;
//        result->target = target;
//
//        return result;
//    }
//
//    void DestroyCamera(camera* cam)
//    {
//        if (cam)
//        {
//            delete cam;
//        }
//    }
//
//    float4x4 getProjectionMatrix(glfw::window* window, camera* cam)
//    {
//        int w, h;
//        glfw::GetWindowSize(window, w, h);
//
//        return math::perspectiveFov(glm::radians(cam->fov), (float)w, (float)h, 0.01f, 100.0f);
//    }
//
//    float4x4 getViewMatrix(camera* cam)
//    {
//        return math::lookAt(cam->eye, cam->target, float3(0.0f, 1.0f, 0.0f));
//    }
//
//    void rayFromScreenToCamera(glfw::window* window, camera* cam, float3& o, float3& d)
//    {
//        int w, h;
//        glfw::GetWindowSize(window, w, h);
//
//        float mx, my;
//        glfw::GetMousePosition(window, mx, my);
//
//        mx = ((mx / (float)w) * 2.0f - 1.0f) * 0.5f;
//        my = ((my / (float)h) * 2.0f - 1.0f) * 0.5f;
//        my *= -1.0f;
//
//        float4x4 proj = getProjectionMatrix(window, cam);
//        float4x4 view = getViewMatrix(cam);
//
//        float4x4 invVP = math::inverse(proj * view);
//        float4 screenPos = float4(mx, my, 1.0f, 1.0f);
//        float4 worldPos = invVP * screenPos;
//
//        o = cam->eye;
//        d = math::normalize(float3(worldPos));
//    }
//}
//
//// cheap and easy particle buffer for snowstorm
////using particle_buffer = particle_buffer_t<1000>;
//using particle_buffer = particle_buffer_t<100000>;
//
//// standard size height field for snowy thiccness
//using heightfield = heightfield_t<256>;
//
//constexpr const unsigned int buffer_size = sizeof(particle_buffer);
//constexpr const double framerate = 1.0 / 60.0;
//
//gl::vertex_array* CreateWireframeSphere(unsigned int segments, float radius)
//{
//    std::vector<unsigned int> indices;
//    std::vector<float3> vertices;
//
//    // y axis
//    for (unsigned int segment = 0; segment < segments; ++segment)
//    {
//        const float percentage = (float)segment / (float)segments;
//        const float npercentage = (float)(segment + 1.0f) / (float)segments;
//
//        const float angle0 = percentage * math::two_pi<float>();
//        const float angle1 = npercentage * math::two_pi<float>();
//
//        float3 xaxis0 = float3(0.0f, math::cos(angle0) * radius, math::sin(angle0) * radius);
//        float3 xaxis1 = float3(0.0f, math::cos(angle1) * radius, math::sin(angle1) * radius);
//
//        float3 yaxis0 = float3(math::cos(angle0) * radius, 0.0f, math::sin(angle0) * radius);
//        float3 yaxis1 = float3(math::cos(angle1) * radius, 0.0f, math::sin(angle1) * radius);
//
//        float3 zaxis0 = float3(math::cos(angle0) * radius, math::sin(angle0) * radius, 0.0f);
//        float3 zaxis1 = float3(math::cos(angle1) * radius, math::sin(angle1) * radius, 0.0f);
//
//        vertices.push_back(xaxis0); vertices.push_back(xaxis1);
//        vertices.push_back(yaxis0); vertices.push_back(yaxis1);
//        vertices.push_back(zaxis0); vertices.push_back(zaxis1);
//    }
//
//    for (unsigned int index = 0; index < vertices.size() - 1; index += 2)
//    {
//        indices.push_back(index + 0);
//        indices.push_back(index + 1);
//    }
//
//    gl::buffer* indexBuffer = gl::CreateIndexBuffer(sizeof(unsigned int) * indices.size(), &indices.at(0));
//    gl::buffer* vertexBuffer = gl::CreateVertexBuffer(sizeof(float3) * vertices.size(), &vertices.at(0));
//
//    gl::vertex_array* result = gl::CreateVertexArray();
//    gl::SetIndexBuffer(result, indexBuffer, indices.size());
//    gl::SetVertexBuffer(result, vertexBuffer, 0);
//    gl::EnableVertexAttribute(result, 0, 0, 3, sizeof(float3), 0);
//
//    return result;
//}
//
//int main()
//{
//    glfw::initialise();
//    glfw::window* window = glfw::CreateWindow("Compute Shader", 640, 480);
//    glew::initialise();
//
//    //glfw::SetPosition(window, -1900, 200);
//    glfw::SetPosition(window, 1980, 200);
//
//    bool exit = false;
//    glfw::SetWindowCloseCallback(window, [&exit](GLFWwindow* w) -> void
//    {
//        exit = true;
//    });
//
//    // load our shaders
//    gl::program* computeShaderInit = gl::CreateComputeProgramFromFile("shaders\\particle_init.ocs");
//    gl::program* computeShader = gl::CreateComputeProgramFromFile("shaders\\particles.ocs");
//    gl::program* flattenShader = gl::CreateComputeProgramFromFile("shaders\\heightfield_flatten.ocs");
//    gl::program* particleShader = gl::CreateShaderProgramFromFile("shaders\\standard_particle.vs", "shaders\\standard_particle.ps");
//    gl::program* heightfieldShader = gl::CreateShaderProgramFromFile("shaders\\heightfield.vs", "shaders\\heightfield.gs", "shaders\\heightfield.ps");
//    gl::program* colorShader = gl::CreateShaderProgramFromFile("shaders\\color.vs", "shaders\\color.ps");
//
//    particle_buffer* particleData = new particle_buffer;
//    heightfield* heightfieldData = new heightfield;
//    for (unsigned int idx = 0; idx < heightfield::size; ++idx)
//    {
//        heightfieldData->heights[idx].x = (float)(idx % heightfield::width) / (float)heightfield::width;
//        heightfieldData->heights[idx].z = (float)(idx / heightfield::width) / (float)heightfield::width;
//        heightfieldData->heights[idx].y = 0.0f;
//    }
//
//    gl::buffer* particleBuffer = gl::CreateComputeBuffer(sizeof(particle_buffer), particleData, true);
//    gl::buffer* heightfieldBuffer = gl::CreateComputeBuffer(sizeof(heightfield), heightfieldData, true);
//
//    // setup all particle data
//    {
//        gl::BindProgram(computeShaderInit);
//        gl::BindComputeBuffer(particleBuffer, 0);
//        gl::UploadUniform(computeShaderInit, "u_GenBox", float3(20.0f, 20.0f, 20.0f));
//        gl::UploadUniform(computeShaderInit, "u_ParticleCount", float(particle_buffer::element_count));
//
//        gl::DispatchComputeProgram(computeShaderInit, CalculateNextPowerOfTwo(particle_buffer::element_count) / 32, 1, 1);
//        gl::BindProgram(0);
//    }
//
//    gl::vertex_array* particleVertexArray = gl::CreateVertexArray();
//    gl::SetIndexBuffer(particleVertexArray, nullptr, 0u);
//    gl::SetVertexBuffer(particleVertexArray, particleBuffer, 0);
//    gl::EnableVertexAttribute(particleVertexArray, 0, 0, 3, sizeof(particle), offsetof(particle, position));
//    gl::EnableVertexAttribute(particleVertexArray, 0, 1, 1, sizeof(particle), offsetof(particle, rotation));
//
//    std::vector<unsigned int> heightfieldIndices;
//    for (unsigned int x = 0; x < heightfield::width - 1; ++x)
//    {
//        for (unsigned int y = 0; y < heightfield::height - 1; ++y)
//        {
//            const unsigned int index = x + y * heightfield::width;
//            heightfieldIndices.push_back(index + 0); heightfieldIndices.push_back(index + heightfield::width); heightfieldIndices.push_back(index + 1);
//            heightfieldIndices.push_back(index + 1); heightfieldIndices.push_back(index + heightfield::width); heightfieldIndices.push_back(index + heightfield::width + 1);
//        }
//    }
//
//    gl::buffer* heightfieldIndexBuffer = gl::CreateIndexBuffer(sizeof(unsigned int) * heightfieldIndices.size(), &heightfieldIndices.at(0));
//
//    gl::vertex_array* heightfieldVertexArray = gl::CreateVertexArray();
//    gl::SetIndexBuffer(heightfieldVertexArray, heightfieldIndexBuffer, heightfieldIndices.size());
//    gl::SetVertexBuffer(heightfieldVertexArray, heightfieldBuffer, 0);
//    gl::EnableVertexAttribute(heightfieldVertexArray, 0, 0, 4, sizeof(float4), 0u);
//
//    float sphereRadius = 3.0f;
//    gl::vertex_array* wireframeSphere = CreateWireframeSphere(16, sphereRadius);
//    float3 spherePos = float3(0.0f);
//
//    glEnable(GL_DEPTH_TEST);
//
//    eng::timer* frameTimer = eng::CreateTimer();
//    eng::time_mark(frameTimer);
//
//    gf::camera* camera = gf::CreateCamera(70.0f, float3(5.0f), float3(0.0f));
//    float cameraRotation = 0.0f;
//
//    const float maxSnowHeight = 1.5f;
//    const float snowIncrement = 0.005f;
//    
//    while (!exit)
//    {
//        // mark timer and retrieve delta time;
//        float dt = (float)eng::to_seconds(eng::time_mark(frameTimer));
//        glfw::SetTitle(window, ("DeltaTime = " + std::to_string(dt)).c_str());
//
//        //cameraRotation += dt * 10.0f;
//        camera->eye = float3(math::cos(math::radians(cameraRotation)) * 40.0f, 20.0f, math::sin(math::radians(cameraRotation)) * 40.0f);
//
//        float3 origin;
//        float3 direction;
//        gf::rayFromScreenToCamera(window, camera, origin, direction);
//
//        float3 hit;
//        if (math::intersectRayWithPlane(origin, direction, float3(0.0f, 1.0f, 0.0f), 0.0f, hit))
//        {
//            spherePos = hit;
//        }
//
//        if (computeShader)
//        {
//            gl::BindProgram(computeShader);
//            gl::UploadUniform(computeShader, "u_DeltaTime", dt);
//            gl::UploadUniform(computeShader, "u_HeightfieldResolution", float2((float)heightfield::width, (float)heightfield::height));
//            gl::UploadUniform(computeShader, "u_MaxSnowHeight", maxSnowHeight);
//            gl::UploadUniform(computeShader, "u_SnowIncrement", snowIncrement);
//            gl::UploadUniform(computeShader, "u_ParticleCount", particle_buffer::element_count);
//
//            gl::BindComputeBuffer(particleBuffer, 0);
//            gl::BindComputeBuffer(heightfieldBuffer, 1);
//            gl::DispatchComputeProgram(computeShader, CalculateNextPowerOfTwo(particle_buffer::element_count) / 32, 1, 1);
//        }
//
//        if (flattenShader)
//        {
//            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
//
//            float4x4 world = float4x4(1.0f);
//            world = math::translate(world, float3(-20.0f, 0.0f, -20.0f));
//            world = math::scale(world, float3(40.0f, 1.0f, 40.0f));
//
//            gl::BindProgram(flattenShader);
//            gl::UploadUniform(flattenShader, "u_HeightfieldSize", float2((float)heightfield::width, (float)heightfield::height));
//            gl::UploadUniform(flattenShader, "u_SpherePos", spherePos);
//            gl::UploadUniform(flattenShader, "u_SphereRadius", sphereRadius);
//            gl::UploadUniform(flattenShader, "u_HeightfieldWorld", world);
//
//            gl::BindComputeBuffer(heightfieldBuffer, 0);
//            gl::DispatchComputeProgram(flattenShader, CalculateNextPowerOfTwo(heightfield::width) / 32, CalculateNextPowerOfTwo(heightfield::height) / 32, 1);
//        }
//
//        gl::SetClearColor(1.0f, 0.0f, 1.0f, 0.0f);
//        gl::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//        glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
//
//        const float4x4 proj = gf::getProjectionMatrix(window, camera);
//        const float4x4 view = gf::getViewMatrix(camera);
//
//        gl::BindProgram(particleShader);
//        gl::UploadUniform(particleShader, "u_Projection", proj);
//        gl::UploadUniform(particleShader, "u_View", view);
//        gl::UploadUniform(particleShader, "u_Color", float3(0.99f, 0.94f, 1.0f));
//
//        glPointSize(1.5f);
//        gl::BindVertexArray(particleVertexArray);
//        //gl::DrawVertexArray(particleVertexArray, GL_POINTS, particle_buffer::element_count);
//
//        float4x4 world = float4x4(1.0f);
//        world = math::translate(world, float3(-20.0f, 0.0f, -20.0f));
//        world = math::scale(world, float3(40.0f, 1.0f, 40.0f));
//
//        gl::BindProgram(heightfieldShader);
//        gl::UploadUniform(heightfieldShader, "u_Projection", proj);
//        gl::UploadUniform(heightfieldShader, "u_View", view);
//        gl::UploadUniform(heightfieldShader, "u_World", world);
//        gl::UploadUniform(heightfieldShader, "u_MaxSnowHeight", maxSnowHeight);
//
//        glPointSize(4.0f);
//        gl::BindVertexArray(heightfieldVertexArray);
//        gl::DrawVertexArray(heightfieldVertexArray, GL_TRIANGLES, heightfieldIndices.size());
//
//        gl::BindProgram(colorShader);
//        gl::UploadUniform(colorShader, "u_Projection", proj);
//        gl::UploadUniform(colorShader, "u_View", view);
//        gl::UploadUniform(colorShader, "u_World", math::translate(float4x4(1.0f), spherePos));
//        gl::UploadUniform(colorShader, "u_Color", float3(1.0f, 0.1f, 0.15f));
//
//        glLineWidth(1.5f);
//        gl::BindVertexArray(wireframeSphere);
//        gl::DrawVertexArray(wireframeSphere, GL_LINES, 1);
//
//        glfw::PollWindowEvents(window);
//        glfw::SwapBuffers(window);
//
//        while (eng::to_seconds(eng::time_elapsed(frameTimer)) < framerate);
//    }
//
//    gl::DestroyVertexArray(particleVertexArray);
//    gl::DestroyProgram(computeShader);
//
//    glfw::DestroyWindow(window);
//    glfw::terminate();
//
//    return 0;
//}