#pragma once

#include "vulkan/vulkan_core.h"

#ifdef __cplusplus
extern "C" {
#endif

VkResult ctsCreateImage(
    VkDevice device,
    const VkImageCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkImage* pImage
);

void ctsDestroyImage(
    VkDevice device,
    VkImage image,
    const VkAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif