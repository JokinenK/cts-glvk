#pragma once

#include <glad/glad.h>
#include <cts/typedefs/device_memory.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsBuffer {
    CtsDeviceMemory memory;
    GLenum type;
    GLsizei size;
    GLsizei offset;
};

#ifdef __cplusplus
}
#endif