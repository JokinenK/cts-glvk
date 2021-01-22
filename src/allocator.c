#include <stdlib.h>
#include <string.h>
#include <cts/allocator.h>

void* ctsAllocation(
    const CtsAllocationCallbacks* pAllocator,
    size_t size,
    size_t align,
    CtsSystemAllocationScope scope
) {
    void* ptr = NULL;

    if (pAllocator) {
        ptr = pAllocator->allocation(pAllocator->userData, size, align, scope);
    } else {
        ptr = malloc(size);
    }

    memset(ptr, 0, size);
    return ptr;
}

void* ctsReallocation(
    const CtsAllocationCallbacks* pAllocator,
    void* pOriginal,
    size_t size,
    size_t align,
    CtsSystemAllocationScope scope
) {
    if (pAllocator) {
        return pAllocator->reallocation(pAllocator->userData, pOriginal, size, align, scope);
    } else {
        return realloc(pOriginal, size);
    }
}

void ctsFree(
    const CtsAllocationCallbacks* pAllocator,
    void* pMemory
) {
    if (pAllocator) {
        pAllocator->free(pAllocator->userData, pMemory);
    } else {
        free(pMemory);
    }
}

void ctsInternalAllocation(
    const CtsAllocationCallbacks* pAllocator,
    size_t size,
    CtsSystemAllocationScope scope
) {
    if (pAllocator) {
        pAllocator->internalAllocation(pAllocator->userData, size, scope);
    }
}

void ctsInternalFree(
    const CtsAllocationCallbacks* pAllocator,
    size_t size, 
    CtsSystemAllocationScope scope
) {
    if (pAllocator) {
        pAllocator->internalFree(pAllocator->userData, size, scope);
    }
}
