#include <stdlib.h>
#include <cts/allocator.h>

void* ctsAllocation(
    const CtsAllocationCallbacks* pAllocator,
    size_t pSize,
    size_t pAlign,
    CtsSystemAllocationScope pScope
) {
    if (pAllocator) {
        return pAllocator->allocation(pAllocator->userData, pSize, pAlign, pScope);
    }

    return malloc(pSize);
}

void* ctsReallocate(
    const CtsAllocationCallbacks* pAllocator,
    void* pOriginal,
    size_t pSize,
    size_t pAlign,
    CtsSystemAllocationScope pScope
) {
    if (pAllocator) {
        return pAllocator->reallocation(pAllocator->userData, pOriginal, pSize, pAlign, pScope);
    }

    return realloc(pOriginal, pSize);
}

void ctsFree(
    const CtsAllocationCallbacks* pAllocator,
    void* pMemory
) {
    if (pAllocator) {
        return pAllocator->free(pAllocator->userData, pMemory);
    }

    free(pMemory);
}

void ctsInternalAllocation(
    const CtsAllocationCallbacks* pAllocator,
    size_t pSize,
    CtsSystemAllocationScope pScope
) {
    if (pAllocator) {
        return pAllocator->internalAllocation(pAllocator->userData, pSize, pScope);
    }
}

void ctsInternalFree(
    const CtsAllocationCallbacks* pAllocator,
    size_t pSize, 
    CtsSystemAllocationScope pScope
) {
    if (pAllocator) {
        return pAllocator->internalFree(pAllocator->userData, pSize, pScope);
    }
}
