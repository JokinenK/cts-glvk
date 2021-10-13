#pragma once

#include "glad/glad.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsGlTexture {
    GLuint handle;
    GLenum target;
    GLenum format;
    GLenum dataType;
    GLint width;
    GLint height;
    GLint depth;
    GLuint unit;
} CtsGlTexture;

#ifdef __cplusplus
}
#endif