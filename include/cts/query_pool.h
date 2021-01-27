#pragma once

#include <cts/allocator.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateQueryPool(
    CtsDevice device,
    const CtsQueryPoolCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsQueryPool* pQueryPool
);

void ctsDestroyQueryPool(
    CtsDevice device,
    CtsQueryPool queryPool,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif