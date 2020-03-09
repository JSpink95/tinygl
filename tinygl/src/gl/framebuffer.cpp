
#include "gl/framebuffer.h"
#include "gl/texture.h"

#include "glfw/glfw.h"

#include <numeric>

namespace gl
{
    framebuffer* CreateFramebuffer(unsigned int w, unsigned int h, unsigned int attachmentCount, const framebuffer::attachment* attachments, bool withDepth)
    {
        framebuffer* result = new framebuffer;

        glGenFramebuffers(1, &result->id);
        glBindFramebuffer(GL_FRAMEBUFFER, result->id);

        for (unsigned int idx = 0; idx < attachmentCount; ++idx)
        {
            const framebuffer::attachment& attachment = attachments[idx];
            texture* t = CreateTexture2D(w, h, attachment.type, attachment.format, attachment.format, nullptr);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + idx, GL_TEXTURE_2D, t->id, 0);

            result->textures.push_back(t);
        }

        if (withDepth)
        {
            result->depth = CreateTexture2D(w, h, GL_FLOAT, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, nullptr);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, result->depth->id, 0);
        }

        std::vector<GLenum> buffers(attachmentCount);
        std::iota(buffers.begin(), buffers.end(), GL_COLOR_ATTACHMENT0);

        glDrawBuffers(buffers.size(), &buffers.at(0));

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return result;
    }

    void DestroyFramebuffer(framebuffer* fb)
    {
        if (fb != nullptr)
        {
            glDeleteFramebuffers(1, &fb->id);
            // #todo - delete textures
            delete fb;
        }
    }

    void BindFramebuffer(framebuffer* fb)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fb ? fb->id : 0);
    }
}
