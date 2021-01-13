#pragma once

#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateDescriptorSetLayout(
    CtsDevice device,
    const CtsDescriptorSetLayoutCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsDescriptorSetLayout* pDescriptorSetLayout
);

void ctsDestroyDescriptorSetLayout(
    CtsDevice device,
    CtsDescriptorSetLayout descriptorSetLayout,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif