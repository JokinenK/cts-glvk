#pragma once

#include <stdint.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateCommandPool(
    CtsDevice pDevice,
    const CtsCommandPoolCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsCommandPool* pCommandPool
);

void ctsDestroyCommandPool(
    CtsDevice pDevice,
    CtsCommandPool pCommandPool,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif