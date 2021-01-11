#pragma once

#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateDescriptorPool(
    CtsDevice pDevice,
    const CtsDescriptorPoolCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsDescriptorPool* pDescriptorPool
);

void ctsDestroyDescriptorPool(
    CtsDevice pDevice,
    CtsDescriptorPool pDescriptorPool,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif