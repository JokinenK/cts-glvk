#pragma once

#include <stdbool.h>
#include <glad/glad.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsFence {
    GLsync sync;
};

CtsResult ctsCreateFenceImpl(
    CtsDevice pDevice,
    const CtsFenceCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsFence* pFence
);

CtsResult ctsResetFencesImpl(
    CtsDevice pDevice,
    uint32_t pFenceCount,
    const CtsFence* pFences
);

CtsResult ctsGetFenceStatusImpl(
    CtsDevice pDevice,
    CtsFence pFence
);

CtsResult ctsWaitForFencesImpl(
    CtsDevice pDevice,
    uint32_t pFenceCount,
    const CtsFence* pFences,
    CtsBool32 pWaitAll,
    uint64_t pTimeout
);

void ctsDestroyFenceImpl(
    CtsDevice pDevice,
    CtsFence pFence,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif