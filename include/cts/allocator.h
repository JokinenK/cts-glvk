#pragma once

#include <stdint.h>
#include "cts/util/align.h"
#include "vulkan/vulkan_core.h"

#ifdef __cplusplus
extern "C" {
#endif

void* ctsAllocation(
    const VkAllocationCallbacks* pAllocator,
    size_t size,
    size_t align,
    VkSystemAllocationScope allocationScope
);

void* ctsReallocation(
    const VkAllocationCallbacks* pAllocator,
    void* pOriginal,
    size_t size,
    size_t align,
    VkSystemAllocationScope allocationScope
);

void ctsFree(
    const VkAllocationCallbacks* pAllocator,
    void* pMemory
);

void ctsInternalAllocation(
    const VkAllocationCallbacks* pAllocator,
    size_t size,
    VkInternalAllocationType allocationType,
    VkSystemAllocationScope allocationScope
);

void ctsInternalFree(
    const VkAllocationCallbacks* pAllocator,
    size_t size, 
    VkInternalAllocationType allocationType,
    VkSystemAllocationScope allocationScope
);

#ifdef __cplusplus
}
#endif