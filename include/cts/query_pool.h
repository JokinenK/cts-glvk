#pragma once

#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateQueryPool(
    CtsDevice pDevice,
    const CtsQueryPoolCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsQueryPool* pQueryPool
);

void ctsDestroyQueryPool(
    CtsDevice pDevice,
    CtsQueryPool pQueryPool,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif