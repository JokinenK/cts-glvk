#pragma once

#include "vulkan/vulkan_core.h"
#include "private/object_base.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsImageView {
    struct CtsObjectBase base;
    
    struct CtsImage* image;
    VkImageViewType viewType;
    VkImageAspectFlags aspectMask;
    GLuint handle;
    GLenum target;
};

VkResult ctsCreateImageViewImpl(
    VkDevice device,
    const VkImageViewCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkImageView* pImageView
);

void ctsDestroyImageViewImpl(
    VkDevice device,
    VkImageView imageView,
    const VkAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif
