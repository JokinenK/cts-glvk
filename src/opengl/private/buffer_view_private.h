#pragma once

#include <glad/glad.h>
#include <cts/typedefs/buffer.h>
#include <cts/typedefs/enums.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsBufferView {
    GLuint handle;
    GLenum format;
    CtsBuffer buffer;
    CtsDeviceSize offset;
    CtsDeviceSize range;
};

#ifdef __cplusplus
}
#endif
