
#include "gl/shader.h"
#include "gl/texture.h"
#include "glfw/glfw.h"

#include <fstream>
#include <string>

struct string_input_stream
{
    const std::string input;
    unsigned int index;
};

string_input_stream createInputStream(const std::string& input) {
    return { input, 0 };
}

bool read(string_input_stream& stream, unsigned char& value) {
    if (stream.index < stream.input.length()) {
        value = stream.input.at(stream.index++);
        return true;
    }

    return false;
}

std::string readUntilChar(string_input_stream& stream, unsigned char match) {
    std::string result;
    unsigned char value;
    while (read(stream, value) && value != match) {
        result += value;
    }

    return result;
}

std::string readEnclosedString(string_input_stream& stream) {
    std::string result;
    readUntilChar(stream, '\"');

    unsigned char value;
    while (read(stream, value) && value != '"') {
        result += value;
    }

    return result;
}

namespace gl
{
    void DestroyShader(shader* s);

    std::string LoadRawFile(const char* f)
    {
        std::string result;

        std::ifstream file(f);
        if (file.is_open())
        {
            std::string line;
            while (std::getline(file, line)) result += line + "\n";

            file.close();
        }

        return result;
    }

    std::string LoadShaderSourceFromFile(const char* f)
    {
        std::string result;

        std::ifstream file(f);
        if (file.is_open())
        {
            std::string line;
            while (std::getline(file, line))
            {
                if (line.length() > 0 && line.at(0) == '#')
                {
                    string_input_stream stream = createInputStream(line);
                    std::string preprocess = readUntilChar(stream, ' ');
                    if (preprocess == "#include")
                    {
                        std::string lib = LoadRawFile(readEnclosedString(stream).c_str());
                        result += "//--start of included file--//\n";
                        result += "\n" + lib + "\n";
                        result += "//---end of included file---//\n";
                        continue;
                    }
                }

                result += line + "\n";
            }

            file.close();
        }

        return result;
    }

    shader* CreateShader(const char* src, GLenum type)
    {
        shader* s = new shader;

        s->id = glCreateShader(type);
        glShaderSource(s->id, 1, &src, nullptr);
        glCompileShader(s->id);

        GLint success = 0;
        glGetShaderiv(s->id, GL_COMPILE_STATUS, &success);

        if (success == 0)
        {
            GLint length = 0;
            glGetShaderiv(s->id, GL_INFO_LOG_LENGTH, &length);

            std::vector<GLchar> info(length);
            glGetShaderInfoLog(s->id, length, nullptr, &info.at(0));

            printf("%s\n", &info.at(0));

            DestroyShader(s);
            s = nullptr;
        }

        return s;
    }

    void DestroyShader(shader* s)
    {
        if (s != nullptr)
        {
            glDeleteShader(s->id);
            delete s;
        }
    }

    void AddShader(program* p, const char* src, GLenum type)
    {
        if (shader* s = CreateShader(src, type))
        {
            glAttachShader(p->id, s->id);
            //DestroyShader(s);
        }
    }

    Uid FindOrAddUniform(program* p, const char* id)
    {
        if (p != nullptr)
        {
            auto it = p->uniforms.find(std::string(id));
            if (it != p->uniforms.end())
            {
                return it->second;
            }

            Uid uid = glGetUniformLocation(p->id, id);
            p->uniforms.emplace(std::string(id), uid);

            return uid;
        }

        return -1;
    }

    bool LinkProgram(program* p)
    {
        if (p != nullptr)
        {
            glLinkProgram(p->id);

            GLint success = 0;
            glGetProgramiv(p->id, GL_LINK_STATUS, &success);

            if (success == 0)
            {
                GLint length = 0;
                glGetProgramiv(p->id, GL_INFO_LOG_LENGTH, &length);

                if (length > 0)
                {
                    std::vector<GLchar> info(length);
                    glGetProgramInfoLog(p->id, length, nullptr, &info.at(0));

                    printf("%s\n", &info.at(0));
                }

                return false;
            }

            return true;
        }

        return false;
    }

    program* CreateShaderProgramFromSource(const char* vs, const char* ps)
    {
        program* result = new program;
        result->id = glCreateProgram();

        AddShader(result, vs, GL_VERTEX_SHADER);
        AddShader(result, ps, GL_FRAGMENT_SHADER);

        if (!LinkProgram(result))
        {
            DestroyProgram(result);
            result = nullptr;
        }

        return result;
    }

    program* CreateShaderProgramFromSource(const char* vs, const char* gs, const char* ps)
    {
        program* result = new program;
        result->id = glCreateProgram();

        AddShader(result, vs, GL_VERTEX_SHADER);
        AddShader(result, gs, GL_GEOMETRY_SHADER);
        AddShader(result, ps, GL_FRAGMENT_SHADER);

        if (!LinkProgram(result))
        {
            DestroyProgram(result);
            result = nullptr;
        }

        return result;
    }

    program* CreateShaderProgramFromFile(const char* vs, const char* ps)
    {
        std::string vs_src = LoadShaderSourceFromFile(vs);
        std::string ps_src = LoadShaderSourceFromFile(ps);

        return CreateShaderProgramFromSource(vs_src.c_str(), ps_src.c_str());
    }

    program* CreateShaderProgramFromFile(const char* vs, const char* gs, const char* ps)
    {
        std::string vs_src = LoadShaderSourceFromFile(vs);
        std::string gs_src = LoadShaderSourceFromFile(gs);
        std::string ps_src = LoadShaderSourceFromFile(ps);

        return CreateShaderProgramFromSource(vs_src.c_str(), gs_src.c_str(), ps_src.c_str());
    }

    program* CreateComputeProgramFromSource(const char* cs)
    {
        program* result = new program;
        result->id = glCreateProgram();

        AddShader(result, cs, GL_COMPUTE_SHADER);

        if (!LinkProgram(result))
        {
            DestroyProgram(result);
            result = nullptr;
        }

        return result;
    }

    program* CreateComputeProgramFromFile(const char* cs)
    {
        std::string cs_src = LoadShaderSourceFromFile(cs);

        return CreateComputeProgramFromSource(cs_src.c_str());
    }

    void DestroyProgram(program* p)
    {
        if (p)
        {
            glDeleteProgram(p->id);
            delete p;
        }
    }

    void BindProgram(program* p)
    {
        glUseProgram(p ? p->id : 0);
    }

    void DispatchComputeProgram(program* p, unsigned int groupX, unsigned int groupY, unsigned int groupZ)
    {
        if (p != nullptr)
        {
            glDispatchCompute(groupX, groupY, groupZ);
        }
    }

    void UploadUniform(program* p, const char* id, float value)
    {
        Uid uid = FindOrAddUniform(p, id);
        glUniform1f(uid, value);
    }

    void UploadUniformArray(program* p, const char* id, unsigned int count, const float* values)
    {
        Uid uid = FindOrAddUniform(p, id);
        glUniform1fv(uid, count, values);
    }

    void UploadUniform(program* p, const char* id, const float2& xy)
    {
        Uid uid = FindOrAddUniform(p, id);
        glUniform2fv(uid, 1, glm::value_ptr(xy));
    }

    void UploadUniformArray(program* p, const char* id, unsigned int count, const float2* xy)
    {
        Uid uid = FindOrAddUniform(p, id);
        glUniform2fv(uid, count, xy ? glm::value_ptr(xy[0]) : nullptr);
    }

    void UploadUniform(program* p, const char* id, const float3& xyz)
    {
        Uid uid = FindOrAddUniform(p, id);
        glUniform3fv(uid, 1, glm::value_ptr(xyz));
    }

    void UploadUniformArray(program* p, const char* id, unsigned int count, const float3* xyz)
    {
        Uid uid = FindOrAddUniform(p, id);
        glUniform3fv(uid, count, xyz ? glm::value_ptr(xyz[0]) : nullptr);
    }

    void UploadUniform(program* p, const char* id, const float4& xyzw)
    {
        Uid uid = FindOrAddUniform(p, id);
        glUniform4fv(uid, 1, glm::value_ptr(xyzw));
    }

    void UploadUniformArray(program* p, const char* id, unsigned int count, const float4* xyzw)
    {
        Uid uid = FindOrAddUniform(p, id);
        glUniform4fv(uid, count, xyzw ? glm::value_ptr(xyzw[0]) : nullptr);
    }

    void UploadUniform(program* p, const char* id, int value)
    {
        Uid uid = FindOrAddUniform(p, id);
        glUniform1i(uid, value);
    }
    
    void UploadUniform(program* p, const char* id, unsigned int count, const int* values)
    {
        Uid uid = FindOrAddUniform(p, id);
        glUniform1iv(uid, count, values);
    }

    void UploadUniform(program* p, const char* id, const float4x4& matrix)
    {
        Uid uid = FindOrAddUniform(p, id);
        glUniformMatrix4fv(uid, 1, GL_FALSE, math::value_ptr(matrix));
    }

    void UploadUniformArray(program* p, const char* id, unsigned int count, const float4x4* matrices)
    {
        Uid uid = FindOrAddUniform(p, id);
        glUniformMatrix4fv(uid, count, GL_FALSE, matrices ? glm::value_ptr(matrices[0]) : nullptr);
    }

    void UploadUniform(program* p, const char* id, texture* t, int location)
    {
        Uid uid = FindOrAddUniform(p, id);

        BindTexture2D(t, location);
        glUniform1i(uid, location);
    }

    void UploadUniformArray(program* p, const char* id, unsigned int count, texture** textures)
    {
        Uid uid = FindOrAddUniform(p, id);

        for (unsigned int index = 0; index < count; ++index)
        {
            if (const texture* t = textures[index])
            {
                //glBindTextureUnit(index, t->id);
                glActiveTexture(GL_TEXTURE0 + index);
                glBindTexture(GL_TEXTURE_2D, t->id);
            }
        }
    }
}