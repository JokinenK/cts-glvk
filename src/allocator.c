#include <stdlib.h>
#include <string.h>
#include "cts/allocator.h"

void* ctsAllocation(
    const VkAllocationCallbacks* pAllocator,
    size_t size,
    size_t align,
    VkSystemAllocationScope allocationScope
) {
    void* ptr = NULL;

    if (pAllocator) {
        ptr = pAllocator->pfnAllocation(pAllocator->pUserData, size, align, allocationScope);
    } else {
        ptr = malloc(size);
    }

    memset(ptr, 0, size);
    return ptr;
}

void* ctsReallocation(
    const VkAllocationCallbacks* pAllocator,
    void* pOriginal,
    size_t size,
    size_t align,
    VkSystemAllocationScope allocationScope
) {
    if (pAllocator) {
        return pAllocator->pfnReallocation(pAllocator->pUserData, pOriginal, size, align, allocationScope);
    } else {
        return realloc(pOriginal, size);
    }
}

void ctsFree(
    const VkAllocationCallbacks* pAllocator,
    void* pMemory
) {
    if (pAllocator) {
        pAllocator->pfnFree(pAllocator->pUserData, pMemory);
    } else {
        free(pMemory);
    }
}

void ctsInternalAllocation(
    const VkAllocationCallbacks* pAllocator,
    size_t size,
    VkInternalAllocationType allocationType,
    VkSystemAllocationScope allocationScope
) {
    if (pAllocator) {
        pAllocator->pfnInternalAllocation(pAllocator->pUserData, size, allocationType, allocationScope);
    }
}

void ctsInternalFree(
    const VkAllocationCallbacks* pAllocator,
    size_t size,
    VkInternalAllocationType allocationType,
    VkSystemAllocationScope allocationScope
) {
    if (pAllocator) {
        pAllocator->pfnInternalFree(pAllocator->pUserData, size, allocationType, allocationScope);
    }
}
