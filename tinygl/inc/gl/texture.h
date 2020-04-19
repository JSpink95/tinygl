
#pragma once

#include "gl/core.h"

namespace gl
{
    struct texture
    {
        struct parameter
        {
            static const unsigned int min_filter;
            static const unsigned int max_filter;
            static const unsigned int wrap_s;
            static const unsigned int wrap_t;

            struct filter
            {
                static const unsigned int linear;
                static const unsigned int point;
            };

            struct wrap
            {
                static const unsigned int repeat;
                static const unsigned int clamp;
            };
        };
        Id id;
    };

    texture* CreateTexture2D(unsigned int w, unsigned int h, unsigned int type, unsigned int internalFormat, unsigned int format, const void* data);
    texture* CreateTexture2D(const char* file);
    void DestroyTexture(texture* t);

    void BindTexture(texture* t, unsigned int unit);
    void BindTexture2D(texture* t, unsigned int location);
    void SetTexture2DParameter(texture* t, unsigned int parameter, unsigned int value);
}