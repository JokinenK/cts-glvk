#pragma once

#include "glad/glad.h"
#include "vulkan/vulkan_core.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsBufferView {
    GLuint handle;
    GLenum format;
    struct CtsBuffer* buffer;
    VkDeviceSize offset;
    VkDeviceSize range;
};

#ifdef __cplusplus
}
#endif
