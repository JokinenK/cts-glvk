#pragma once

#include <stdbool.h>
#include <glad/glad.h>
#include <cts/fence.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsFenceImpl {
    GLsync sync;
    GLint status;
};

void ctsSignalFence(
    CtsDevice device,
    CtsFence fence
);

CtsResult ctsCreateFenceImpl(
    CtsDevice device,
    const CtsFenceCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsFence* pFence
);

CtsResult ctsResetFencesImpl(
    CtsDevice device,
    uint32_t fenceCount,
    const CtsFence* pFences
);

CtsResult ctsGetFenceStatusImpl(
    CtsDevice device,
    CtsFence fence
);

void ctsSignalFenceFenceImpl(
    CtsDevice device,
    CtsFence fence
);

CtsResult ctsWaitForFencesImpl(
    CtsDevice device,
    uint32_t fenceCount,
    const CtsFence* pFences,
    CtsBool32 waitAll,
    uint64_t timeout
);

void ctsDestroyFenceImpl(
    CtsDevice device,
    CtsFence fence,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif