#pragma once

#include <stdint.h>
#include <glad/glad.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsGlShaderStage {
    GLuint handle;
    const char* pName;
} CtsGlShaderStage;

typedef struct CtsGlShader {
    GLuint handle;
    uint32_t stageCount;
    CtsGlShaderStage* pStages;
} CtsGlShader;

#ifdef __cplusplus
}
#endif