#include <stddef.h>
#include <cts/pipeline_layout.h>
#include <private/pipeline_layout_private.h>
#include <private/descriptor_set_layout_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreatePipelineLayout(
    CtsDevice pDevice,
    const CtsPipelineLayoutCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsPipelineLayout* pPipelineLayout
) {
    (void) pDevice;

    CtsPipelineLayout pipelineLayout = ctsAllocation(
        pAllocator,
        sizeof(struct CtsPipelineLayout),
        alignof(struct CtsPipelineLayout),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (pipelineLayout == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    pipelineLayout->setLayoutCount = pCreateInfo->setLayoutCount;
    pipelineLayout->setLayouts = ctsAllocation(
        pAllocator,
        sizeof(struct CtsDescriptorSetLayout) * pipelineLayout->setLayoutCount,
        alignof(struct CtsDescriptorSetLayout),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    for (uint32_t i = 0; i < pipelineLayout->setLayoutCount; ++i) {
        pipelineLayout->setLayouts[i] = pCreateInfo->setLayouts[i];
    }

    pipelineLayout->pushConstantRangeCount = pCreateInfo->pushConstantRangeCount;
    pipelineLayout->pushConstantRanges = ctsAllocation(
        pAllocator,
        sizeof(struct CtsPushConstantRange) * pipelineLayout->pushConstantRangeCount,
        alignof(struct CtsPushConstantRange),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    for (uint32_t i = 0; i < pipelineLayout->pushConstantRangeCount; ++i) {
        pipelineLayout->pushConstantRanges[i] = pCreateInfo->pushConstantRanges[i];
    }

    *pPipelineLayout = pipelineLayout;
    return CTS_SUCCESS;
}

void ctsDestroyPipelineLayout(
    CtsDevice pDevice,
    CtsPipelineLayout pPipelineLayout,
    const CtsAllocationCallbacks* pAllocator
) {
    (void) pDevice;

    if (pPipelineLayout != NULL) {
        ctsFree(pAllocator, pPipelineLayout->setLayouts);
        ctsFree(pAllocator, pPipelineLayout->pushConstantRanges);
        ctsFree(pAllocator, pPipelineLayout);
    }
}

#ifdef __cplusplus
}
#endif