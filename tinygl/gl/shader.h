
#pragma once

#include "gl/core.h"
#include "maths/maths.h"

#include <unordered_map>

namespace gl
{
    using Uid = int;

    struct texture;

    struct shader
    {
        Id id;
    };

    struct program
    {
        Id id;
        std::unordered_map<std::string, Uid> uniforms;
    };

    program* CreateShaderProgramFromSource(const char* vs, const char* ps);
    program* CreateShaderProgramFromSource(const char* vs, const char* gs, const char* ps);

    program* CreateShaderProgramFromFile(const char* vs, const char* ps);
    program* CreateShaderProgramFromFile(const char* vs, const char* gs, const char* ps);

    program* CreateComputeProgramFromSource(const char* cs);
    program* CreateComputeProgramFromFile(const char* cs);

    void DestroyProgram(program* p);

    void BindProgram(program* p);
    void DispatchComputeProgram(program* p, unsigned int groupX, unsigned int groupY, unsigned int groupZ);

    // floats
    void UploadUniform(program* p, const char* id, float value);
    void UploadUniformArray(program* p, const char* id, unsigned int count, const float* value);

    void UploadUniform(program* p, const char* id, const float2& xy);
    void UploadUniformArray(program* p, const char* id, unsigned int count, const float2* xy);

    void UploadUniform(program* p, const char* id, const float3& xyz);
    void UploadUniformArray(program* p, const char* id, unsigned int count, const float3& xyz);

    void UploadUniform(program* p, const char* id, const float4& xyzw);
    void UploadUniformArray(program* p, const char* id, unsigned int count, const float4& xyzw);

    // matrices
    void UploadUniform(program* p, const char* id, const float4x4& matrix);
    void UploadUniformArray(program* p, const char* id, unsigned int count, const float4x4* matrix);

    // textures
    void UploadUniform(program* p, const char* id, texture* t, int location);
}
