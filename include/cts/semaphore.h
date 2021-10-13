#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "vulkan/vulkan_core.h"

#ifdef __cplusplus
extern "C" {
#endif

VkResult ctsCreateSemaphore(
    VkDevice device,
    const VkSemaphoreCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSemaphore* pSemaphore
);

void ctsDestroySemaphore(
    VkDevice device,
    VkSemaphore semaphore,
    const VkAllocationCallbacks* pAllocator
);

VkResult ctsWaitSemaphore(
    VkSemaphore semaphore,
    uint64_t timeout
);

void ctsWaitSemaphores(
    uint32_t semaphoreCount,
    const VkSemaphore* pSemaphores
);

void ctsSignalSemaphores(
    uint32_t semaphoreCount,
    const VkSemaphore* pSemaphores
);

#ifdef __cplusplus
}
#endif