#pragma once

#include "vulkan/vk_layer.h"
#include "vulkan/vulkan_core.h"
#include "cts/macros.h"

#ifdef __cplusplus
extern "C" {
#endif

VkResult VKAPI_CALL ctsCreateFence(
    VkDevice device,
    const VkFenceCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkFence* pFence
);

VkResult VKAPI_CALL ctsResetFences(
    VkDevice device,
    uint32_t fenceCount,
    const VkFence* pFences
);

VkResult VKAPI_CALL ctsGetFenceStatus(
    VkDevice device,
    VkFence fence
);

VkResult VKAPI_CALL ctsWaitForFences(
    VkDevice device,
    uint32_t fenceCount,
    const VkFence* pFences,
    VkBool32 waitAll,
    uint64_t timeout
);

void VKAPI_CALL ctsDestroyFence(
    VkDevice device,
    VkFence fence,
    const VkAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif