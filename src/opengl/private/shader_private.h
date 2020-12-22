#pragma once

#include <glad/glad.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsShaderModule {
    GLint codeSize;
    GLchar* code;
};

#ifdef __cplusplus
}
#endif