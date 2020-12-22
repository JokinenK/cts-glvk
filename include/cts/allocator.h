#pragma once

#include <stdint.h>
#include <cts/align.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum CtsSystemAllocationScope {
    CTS_SYSTEM_ALLOCATION_SCOPE_COMMAND = 0,
    CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT = 1,
    CTS_SYSTEM_ALLOCATION_SCOPE_CACHE = 2,
    CTS_SYSTEM_ALLOCATION_SCOPE_DEVICE = 3,
    CTS_SYSTEM_ALLOCATION_SCOPE_INSTANCE = 4,
} CtsSystemAllocationScope;

typedef struct CtsAllocationCallbacks {
    void* userData;

    void*(*allocation)(
        void* pUserData,
        size_t pSize,
        size_t pAlign,
        CtsSystemAllocationScope pScope
    );

    void*(*reallocation)(
        void* pUserData,
        void* pOriginal,
        size_t pSize,
        size_t pAlign,
        CtsSystemAllocationScope pScope
    );

    void(*free)(
        void* pUserData,
        void* pMemory
    );

    void(*internalAllocation)(
        void* pUserData,
        size_t pSize,
        CtsSystemAllocationScope pScope
    );

    void(*internalFree)(
        void* pUserData,
        size_t pSize, 
        CtsSystemAllocationScope pScope
    );
} CtsAllocationCallbacks;

void* ctsAllocation(
    const CtsAllocationCallbacks* pAllocator,
    size_t pSize,
    size_t pAlign,
    CtsSystemAllocationScope pScope
);

void* ctsReallocation(
    const CtsAllocationCallbacks* pAllocator,
    void* pOriginal,
    size_t pSize,
    size_t pAlign,
    CtsSystemAllocationScope pScope
);

void ctsFree(
    const CtsAllocationCallbacks* pAllocator,
    void* pMemory
);

void ctsInternalAllocation(
    const CtsAllocationCallbacks* pAllocator,
    size_t pSize,
    CtsSystemAllocationScope pScope
);

void ctsInternalFree(
    const CtsAllocationCallbacks* pAllocator,
    size_t pSize, 
    CtsSystemAllocationScope pScope
);

#ifdef __cplusplus
}
#endif