#pragma once

#include <glad/glad.h>
#include <cts/typedefs/device_memory.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsBufferImpl {
    CtsDeviceMemory memory;
    GLsizei size;
    GLenum type;
    GLsizei offset;
    CtsBufferUsageFlags usage;
};

#ifdef __cplusplus
}
#endif