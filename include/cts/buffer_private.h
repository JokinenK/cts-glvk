#pragma once

#include "glad/glad.h"
#include "vulkan/vulkan_core.h"
#include "cts/object_base.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsBuffer {
    struct CtsObjectBase base;
    struct CtsDeviceMemory* memory;
    GLsizei size;
    GLenum type;
    GLsizei offset;
    VkBufferUsageFlags usage;
};

#ifdef __cplusplus
}
#endif