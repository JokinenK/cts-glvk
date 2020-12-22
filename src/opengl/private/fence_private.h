#pragma once

#include <glad/glad.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsFence {
    GLsync sync;
    GLenum flags;
};

#ifdef __cplusplus
}
#endif