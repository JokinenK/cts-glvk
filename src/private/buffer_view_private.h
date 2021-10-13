#pragma once

#include "glad/glad.h"
#include "vulkan/vulkan_core.h"
#include "private/object_base.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsBufferView {
    struct CtsObjectBase base;
    GLuint handle;
    GLenum format;
    struct CtsBuffer* buffer;
    VkDeviceSize offset;
    VkDeviceSize range;
};

#ifdef __cplusplus
}
#endif
