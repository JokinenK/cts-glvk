#pragma once

#include <stdbool.h>
#include <glad/glad.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsFence {
    GLsync sync;
};

#ifdef __cplusplus
}
#endif