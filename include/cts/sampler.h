#pragma once

#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateSampler(
    CtsDevice pDevice,
    const CtsSamplerCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsSampler* pSampler
);

void ctsDestroySampler(
    CtsDevice pDevice,
    CtsSampler pSampler,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif