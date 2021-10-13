#pragma once

#include "vulkan/vulkan_core.h"

#ifdef __cplusplus
extern "C" {
#endif

VkResult ctsCreateFence(
    VkDevice device,
    const VkFenceCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkFence* pFence
);

VkResult ctsResetFences(
    VkDevice device,
    uint32_t fenceCount,
    const VkFence* pFences
);

VkResult ctsGetFenceStatus(
    VkDevice device,
    VkFence fence
);

VkResult ctsWaitForFences(
    VkDevice device,
    uint32_t fenceCount,
    const VkFence* pFences,
    VkBool32 waitAll,
    uint64_t timeout
);

void ctsDestroyFence(
    VkDevice device,
    VkFence fence,
    const VkAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif