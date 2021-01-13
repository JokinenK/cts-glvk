#pragma once

#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateDescriptorPool(
    CtsDevice device,
    const CtsDescriptorPoolCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsDescriptorPool* pDescriptorPool
);

void ctsDestroyDescriptorPool(
    CtsDevice device,
    CtsDescriptorPool descriptorPool,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif