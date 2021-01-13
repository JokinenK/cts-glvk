#include <stdint.h>
#include <stdio.h>
#include <glad/glad.h>
#include <cts/fullscreen_texture.h>
#include <cts/typedefs/gl_shader.h>
#include <cts/typedefs/gl_pipeline.h>
#include <private/device_private.h>
#include <private/framebuffer_private.h>
#include <private/image_private.h>
#include <private/pipeline_private.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SHADER_MODULE_COUNT 2

static const char gVsShaderSource[] = 
    "layout(location = 0) out vec2 uv;\n"
    "\n"
    "void main() {\n"
    "    float x = float(((uint(gl_VertexID) + 2u) / 3u) % 2u);\n"
    "    float y = float(((uint(gl_VertexID) + 1u) / 3u) % 2u);\n"
    "\n"
    "    gl_Position = vec4(-1.0f + x * 2.0f, -1.0f + y * 2.0f, 0.0f, 1.0f);\n"
    "    uv = vec2(x, y);\n"
    "}\n";

static const char gFsShaderSource[] = 
    "layout(location = 0) in vec2 uv;\n"
    "layout(location = 0) out vec4 fragColor;\n"
    "\n"
    "uniform sampler2D sampler;\n"
    "\n"
    "void main() {\n"
    "    fragColor = vec4(texture2D(sampler, uv), 1);\n"
    "}\n";

static int gReferenceCount = 0;
static GLenum gShaderProgram = GL_INVALID_ENUM;
struct ShaderData {
    GLenum handle;
    GLenum type;
    const char* source;
    const int sourceLen;
} gShaderModules[SHADER_MODULE_COUNT] = {
    { GL_INVALID_ENUM, GL_VERTEX_SHADER,   gVsShaderSource, sizeof(gVsShaderSource) },
    { GL_INVALID_ENUM, GL_FRAGMENT_SHADER, gFsShaderSource, sizeof(gFsShaderSource) },
};

CtsResult ctsInitFSTextureHelper() {
    if (++gReferenceCount > 1) {
        return CTS_SUCCESS;
    }
    
    GLint success;
    GLchar buffer[512];

    gShaderProgram = glCreateProgram();

    for (uint32_t i = 0; i < SHADER_MODULE_COUNT; ++i) {
        struct ShaderData* data = &gShaderModules[i];

        data->handle = glCreateShader(data->type);
        glShaderSource(data->handle, 1, &data->source, &data->sourceLen);
        glCompileShader(data->handle);
        glGetShaderiv(data->handle, GL_COMPILE_STATUS, &success);

        if (success) {
            glAttachShader(gShaderProgram, data->handle);
        }
        else {
            glGetShaderInfoLog(data->handle, sizeof(buffer), NULL, buffer);
            fprintf(stderr, "Shader compilation failed for type %s. Reason %s", data->type, buffer);
        }
    }

    glLinkProgram(gShaderProgram);
    glGetProgramiv(gShaderProgram, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(gShaderProgram, sizeof(buffer), NULL, buffer);
        fprintf(stderr, "Shader linking failed: %s", buffer);
        return CTS_NOT_READY;
    }

    GLint samplerLocation = glGetUniformLocation(gShaderProgram, "sampler");
    glUniform1i(samplerLocation, 0);
    
    return CTS_SUCCESS;
}

CtsResult ctsDrawFSTexture(
    CtsDevice device,
    CtsImage image
) {
    GLuint prevProgram = (device->activeGraphicsPipeline != NULL)
        ? device->activeGraphicsPipeline->shader.handle
        : 0u;

    GLuint prevFramebuffer = (device->activeFramebuffer != NULL)
        ? device->activeFramebuffer->handle
        : 0u;

    GLenum prevTextureTarget = device->activeTextures[0].target;
    GLint prevTextureHandle = device->activeTextures[0].handle;

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glUseProgram(gShaderProgram);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, image->handle);
    glDrawArrays(GL_POINT, 0, 4);
    glUseProgram(prevProgram);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, prevFramebuffer);
    glBindTexture(prevTextureTarget, prevTextureHandle);

    return CTS_SUCCESS;
}

void ctsCleanupFSTextureHelper() {
    if (--gReferenceCount == 0) {
        for (uint32_t i = 0; i < SHADER_MODULE_COUNT; ++i) {
            glDeleteShader(gShaderModules[i].handle);
        }

        glDeleteProgram(gShaderProgram);
    }
}

#ifdef __cplusplus
}
#endif