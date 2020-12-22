#pragma once

#include <glad/glad.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsBuffer {
    GLuint handle;
    GLenum type;
    GLsizei size;
    GLsizei offset;
};

#ifdef __cplusplus
}
#endif