#pragma once

#include <glad/glad.h>
#include <cts/typedefs/device_size.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsDeviceMemory {
    GLenum handle;
    CtsDeviceSize size;
};

#ifdef __cplusplus
}
#endif