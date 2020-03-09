
#include "gl/texture.h"

#include "glfw/glfw.h"

namespace gl
{
    const unsigned int texture::parameter::min_filter = GL_TEXTURE_MIN_FILTER;
    const unsigned int texture::parameter::max_filter = GL_TEXTURE_MAG_FILTER;
    const unsigned int texture::parameter::wrap_s = GL_TEXTURE_WRAP_S;
    const unsigned int texture::parameter::wrap_t = GL_TEXTURE_WRAP_T;

    const unsigned int texture::parameter::filter::linear = GL_LINEAR;
    const unsigned int texture::parameter::filter::point = GL_NEAREST;

    const unsigned int texture::parameter::wrap::repeat = GL_REPEAT;
    const unsigned int texture::parameter::wrap::clamp = GL_CLAMP_TO_EDGE;

    texture* CreateTexture2D(unsigned int w, unsigned int h, unsigned int type, unsigned int internalFormat, unsigned int format, const void* data)
    {
        texture* result = new texture;

        glGenTextures(1, &result->id);
        glBindTexture(GL_TEXTURE_2D, result->id);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, type, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glBindTexture(GL_TEXTURE_2D, 0);

        return result;
    }

    void DestroyTexture(texture* t)
    {
        if (t)
        {
            glDeleteTextures(1, &t->id);
            delete t;
        }
    }

    void BindTexture(texture* t, unsigned int unit)
    {
        glBindTextureUnit(unit, t ? t->id : 0);
    }
    
    void BindTexture2D(texture* t, unsigned int location)
    {
        glActiveTexture(GL_TEXTURE0 + location);
        glBindTexture(GL_TEXTURE_2D, t ? t->id : 0);
    }

    void SetTexture2DParameter(texture* t, unsigned int parameter, unsigned int value)
    {
        glBindTexture(GL_TEXTURE_2D, t->id);
        glTexParameteri(GL_TEXTURE_2D, parameter, value);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}