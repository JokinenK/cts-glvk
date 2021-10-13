#pragma once

#include "glad/glad.h"
#include "vulkan/vulkan_core.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsBuffer {
    struct CtsDeviceMemory* memory;
    GLsizei size;
    GLenum type;
    GLsizei offset;
    VkBufferUsageFlags usage;
};

#ifdef __cplusplus
}
#endif