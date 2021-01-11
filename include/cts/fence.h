#pragma once

#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateFence(
    CtsDevice pDevice,
    const CtsFenceCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsFence* pFence
);

CtsResult ctsResetFences(
    CtsDevice pDevice,
    uint32_t pFenceCount,
    const CtsFence* pFences
);

CtsResult ctsGetFenceStatus(
    CtsDevice pDevice,
    CtsFence pFence
);

CtsResult ctsWaitForFences(
    CtsDevice pDevice,
    uint32_t pFenceCount,
    const CtsFence* pFences,
    CtsBool32 pWaitAll,
    uint64_t pTimeout
);

void ctsDestroyFence(
    CtsDevice pDevice,
    CtsFence pFence,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif