#pragma once

#include "vulkan/vk_layer.h"
#include "vulkan/vulkan_core.h"
#include "cts/macros.h"

#ifdef __cplusplus
extern "C" {
#endif

VkResult VKAPI_CALL ctsCreateImage(
    VkDevice device,
    const VkImageCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkImage* pImage
);

void VKAPI_CALL ctsDestroyImage(
    VkDevice device,
    VkImage image,
    const VkAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif