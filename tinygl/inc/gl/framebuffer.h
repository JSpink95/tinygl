
#pragma once

#include "gl/core.h"

#include <vector>

namespace gl
{
    struct texture;
    struct framebuffer
    {
        struct attachment
        {
            unsigned int format;
            unsigned int type;
        };

        Id id;
        std::vector<texture*> textures;
        texture* depth;
    };

    framebuffer* CreateFramebuffer(unsigned int w, unsigned int h, unsigned int attachmentCount, const framebuffer::attachment* attachments, bool withDepth);
    void DestroyFramebuffer(framebuffer* fb);

    void BindFramebuffer(framebuffer* fb);
}
