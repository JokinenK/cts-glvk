#pragma once

#include <stdint.h>
#include <glad/glad.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsGlShaderStage {
    GLuint handle;
    const char* name;
} CtsGlShaderStage;

typedef struct CtsGlShader {
    GLuint handle;
    uint32_t stageCount;
    CtsGlShaderStage* stages;
} CtsGlShader;

#ifdef __cplusplus
}
#endif