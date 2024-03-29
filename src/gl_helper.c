#include <stdint.h>
#include <stdio.h>
#include "cts/macros.h"
#include "cts/type_mapper.h"
#include "cts/gl_helper.h"
#include "cts/gl_shader.h"
#include "cts/gl_pipeline.h"
#include "cts/private.h"
#include "cts/device_private.h"
#include "cts/framebuffer_private.h"
#include "cts/image_private.h"
#include "cts/pipeline_private.h"

#ifdef __cplusplus
extern "C" {
#endif

static GLenum parseAttachment(VkImageAspectFlags aspectFlags);
static GLenum parseMask(VkImageAspectFlags aspectFlags);

static const char gVsShaderSource[] = 
    "#version 330\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "\n"
    "layout(location = 0) out vec2 uv;\n"
    "\n"
    "void main() {\n"
    "    float x = float(((uint(gl_VertexID) + 2u) / 3u) % 2u);\n"
    "    float y = 1.0f - float(((uint(gl_VertexID) + 1u) / 3u) % 2u);\n"
    "\n"
    "    gl_Position = vec4(-1.0f + x * 2.0f, -1.0f + y * 2.0f, 0.0f, 1.0f);\n"
    "    uv = vec2(x, -y);\n"
    "}\n";

static const char gFsShaderSource[] = 
    "#version 330\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "\n"
    "layout(location = 0) in vec2 uv;\n"
    "layout(location = 0) out vec4 fragColor;\n"
    "\n"
    "uniform sampler2D sampler;\n"
    "\n"
    "void main() {\n"
    "    fragColor = texture(sampler, uv);\n"
    "}\n";


struct ShaderSource {
    GLenum type;
    const char* source;
    const int sourceLen;
} gShaderSources[] = {
    { GL_VERTEX_SHADER,   gVsShaderSource, sizeof(gVsShaderSource) },
    { GL_FRAGMENT_SHADER, gFsShaderSource, sizeof(gFsShaderSource) },
};

static int gReferenceCount = 0;
static GLuint gShaderProgram = GL_INVALID_VALUE;
static GLuint gReadFramebuffer = GL_INVALID_VALUE;
static GLuint gWriteFramebuffer = GL_INVALID_VALUE;
static GLuint gShaders[CTS_ARRAY_SIZE(gShaderSources)];

bool ctsInitGlHelper() {
    if (gReferenceCount++ > 0) {
        return true;
    }

    GLint success;
    GLchar buffer[512];

    glEnable(GL_FRAMEBUFFER_SRGB); 
    gShaderProgram = glCreateProgram();

    glGenFramebuffers(1, &gReadFramebuffer);
    glGenFramebuffers(1, &gWriteFramebuffer);

    for (uint32_t i = 0; i < CTS_ARRAY_SIZE(gShaderSources); ++i) {
        struct ShaderSource* source = &gShaderSources[i];

        GLenum handle = glCreateShader(source->type);
        glShaderSource(handle, 1, &source->source, &source->sourceLen);
        glCompileShader(handle);
        glGetShaderiv(handle, GL_COMPILE_STATUS, &success);

        if (success) {
            glAttachShader(gShaderProgram, handle);
        }
        else {
            glGetShaderInfoLog(handle, sizeof(buffer), NULL, buffer);
            fprintf(stderr, "Shader compilation failed for type %d. Reason %s", source->type, buffer);
        }

        gShaders[i] = handle;
    }

    glLinkProgram(gShaderProgram);
    glGetProgramiv(gShaderProgram, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(gShaderProgram, sizeof(buffer), NULL, buffer);
        fprintf(stderr, "Shader linking failed: %s", buffer);
        return false;
    }

    glUseProgram(gShaderProgram);
    glUniform1i(glGetUniformLocation(gShaderProgram, "sampler"), 0);
    glUseProgram(0);
    glActiveTexture(GL_TEXTURE0);
    
    return true;
}

void ctsDestroyGlHelper(struct CtsGlHelper* helper) {
    if (--gReferenceCount == 0) {
        for (uint32_t i = 0; i < CTS_ARRAY_SIZE(gShaderSources); ++i) {
            glDeleteShader(gShaders[i]);
        }

        glDeleteFramebuffers(1, &gReadFramebuffer);
        glDeleteFramebuffers(1, &gWriteFramebuffer);
        glDeleteProgram(gShaderProgram);
    }
}

void ctsGlHelperBlitTexture(
    struct CtsDevice* device,
    struct CtsImage* src,
    struct CtsImage* dst,
    uint32_t regionCount,
    const VkImageBlit* pRegions,
    VkFilter filter
) {
    GLuint prevReadFramebuffer = (device->activeReadFramebuffer != NULL)
        ? device->activeReadFramebuffer->handle
        : 0u;

    GLuint prevWriteFramebuffer = (device->activeWriteFramebuffer != NULL)
        ? device->activeWriteFramebuffer->handle
        : 0u;

    glBindFramebuffer(GL_READ_FRAMEBUFFER, gReadFramebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gWriteFramebuffer);

    for (uint32_t i = 0; i < regionCount; ++i) {
        const VkImageBlit* pRegion = &pRegions[i];

        GLuint srcMask = parseMask(pRegions->srcSubresource.aspectMask);
        GLuint dstMask = parseMask(pRegions->dstSubresource.aspectMask);

        if ((srcMask & dstMask) == 0) {
            continue;
        }

        GLenum srcAttachment = parseAttachment(pRegions->srcSubresource.aspectMask);
        GLenum dstAttachment = parseAttachment(pRegions->dstSubresource.aspectMask);

        glFramebufferTexture2D(GL_READ_FRAMEBUFFER, srcAttachment, src->target, src->handle, pRegions->srcSubresource.mipLevel);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, dstAttachment, dst->target, dst->handle, pRegions->dstSubresource.mipLevel);

        glReadBuffer(srcAttachment);
        glDrawBuffer(dstAttachment);

        glBlitFramebuffer(
            pRegions->srcOffsets[0].x,
            pRegions->srcOffsets[0].y,
            pRegions->srcOffsets[1].x,
            pRegions->srcOffsets[1].y,
            pRegions->dstOffsets[0].x,
            pRegions->dstOffsets[0].y,
            pRegions->dstOffsets[1].x,
            pRegions->dstOffsets[1].y,
            srcMask & dstMask,
            parseMagFilter(filter, VK_DESCRIPTOR_TYPE_MAX_ENUM) // Enum is ignored
        );
    }

    glBindFramebuffer(GL_READ_FRAMEBUFFER, prevReadFramebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, prevWriteFramebuffer);
}

void ctsGlDrawFinalImage(
    struct CtsDevice* device,
    struct CtsImage* image
) {
    GLenum prevTarget = device->state.texture[0].target;
    GLint prevTexture = device->state.texture[0].texture;
    GLuint prevFramebuffer = (device->activeWriteFramebuffer != NULL)
        ? device->activeWriteFramebuffer->handle
        : 0u;

    GLuint prevProgram = device->state.program;
    GLboolean prevCullFace = device->state.cullFace;
    GLboolean prevDepthTest = device->state.depthTest;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, image->handle);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glUseProgram(gShaderProgram);

    if (prevCullFace) { glDisable(GL_CULL_FACE); }
    if (prevDepthTest) { glDisable(GL_DEPTH_TEST); }
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    if (prevCullFace) { glEnable(GL_CULL_FACE); }
    if (prevDepthTest) { glEnable(GL_DEPTH_TEST); }

    glUseProgram(prevProgram);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, prevFramebuffer);

    if (prevTarget != GL_NONE) {
        glBindTexture(prevTarget, prevTexture);
    }
}


static GLenum parseAttachment(VkImageAspectFlags aspectFlags) {
    if (aspectFlags & VK_IMAGE_ASPECT_COLOR_BIT) {
        return GL_COLOR_ATTACHMENT0;
    } else if (aspectFlags & VK_IMAGE_ASPECT_DEPTH_BIT && aspectFlags & VK_IMAGE_ASPECT_STENCIL_BIT) {
        return GL_DEPTH_STENCIL_ATTACHMENT;
    } else if (aspectFlags & VK_IMAGE_ASPECT_DEPTH_BIT) {
        return GL_DEPTH_ATTACHMENT;
    } else if (aspectFlags & VK_IMAGE_ASPECT_STENCIL_BIT) {
        return GL_STENCIL_ATTACHMENT;
    }

    return GL_INVALID_ENUM;
}

static GLenum parseMask(VkImageAspectFlags aspectFlags) {
    GLuint flags = 0;

    if (aspectFlags & VK_IMAGE_ASPECT_COLOR_BIT) {
        flags |= GL_COLOR_BUFFER_BIT;
    } 
    if (aspectFlags & VK_IMAGE_ASPECT_DEPTH_BIT) {
        flags |= GL_DEPTH_BUFFER_BIT;
    }
    if (aspectFlags & VK_IMAGE_ASPECT_STENCIL_BIT) {
        flags |= GL_STENCIL_BUFFER_BIT;
    }

    return flags;
}

#ifdef __cplusplus
}
#endif