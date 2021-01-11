#pragma oncec

#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateDescriptorSetLayout(
    CtsDevice pDevice,
    const CtsDescriptorSetLayoutCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsDescriptorSetLayout* pDescriptorSetLayout
);

void ctsDestroyDescriptorSetLayout(
    CtsDevice pDevice,
    CtsDescriptorSetLayout pDescriptorSetLayout,
    const CtsAllocationCallbacks* pAllocator
);

CtsResult ctsCreatePipelineLayout(
    CtsDevice pDevice,
    const CtsPipelineLayoutCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsPipelineLayout* pPipelineLayout
);

void ctsDestroyPipelineLayout(
    CtsDevice pDevice,
    CtsPipelineLayout pPipelineLayout,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif