#pragma once

#include <cts/allocator.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateFence(
    CtsDevice device,
    const CtsFenceCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsFence* pFence
);

CtsResult ctsResetFences(
    CtsDevice device,
    uint32_t fenceCount,
    const CtsFence* pFences
);

CtsResult ctsGetFenceStatus(
    CtsDevice device,
    CtsFence fence
);

CtsResult ctsWaitForFences(
    CtsDevice device,
    uint32_t fenceCount,
    const CtsFence* pFences,
    CtsBool32 waitAll,
    uint64_t timeout
);

void ctsDestroyFence(
    CtsDevice device,
    CtsFence fence,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif