#pragma once

#include "glad/glad.h"
#include "vulkan/vulkan_core.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsImage {
    GLenum handle;
    GLenum format;
    GLenum internalFormat;
    GLenum type;
    GLenum target;
    VkImageType imageType;
    VkImageUsageFlags imageUsage;

    GLint width;
    GLint height;
    GLint depth;
    GLint arrayLayers;
    GLint mipLevels;
    GLint numComponents;
    GLint samples;

    VkDeviceSize size;
    VkDeviceSize offset;
    struct CtsImage* nextSibling;
    struct CtsDeviceMemory* memory;
};

VkResult ctsCreateImageImpl(
    VkDevice device,
    const VkImageCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkImage* pImage
);

void ctsDestroyImageImpl(
    VkDevice device,
    VkImage image,
    const VkAllocationCallbacks* pAllocator
);

uintptr_t ctsImageToBuffer(struct CtsImage* image, uintptr_t offset);
uintptr_t ctsBufferToImage(struct CtsImage* image, uintptr_t offset);

#ifdef __cplusplus
}
#endif
