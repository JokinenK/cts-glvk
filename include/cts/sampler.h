#pragma once

#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateSampler(
    CtsDevice device,
    const CtsSamplerCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsSampler* pSampler
);

void ctsDestroySampler(
    CtsDevice device,
    CtsSampler sampler,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif