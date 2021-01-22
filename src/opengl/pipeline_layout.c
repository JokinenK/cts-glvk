#include <stddef.h>
#include <cts/pipeline_layout.h>
#include <private/pipeline_layout_private.h>
#include <private/descriptor_set_layout_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreatePipelineLayout(
    CtsDevice device,
    const CtsPipelineLayoutCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsPipelineLayout* pPipelineLayout
) {
    (void) device;

    CtsPipelineLayout pipelineLayout = ctsAllocation(
        pAllocator,
        sizeof(struct CtsPipelineLayoutImpl),
        alignof(struct CtsPipelineLayoutImpl),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (pipelineLayout == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    pipelineLayout->setLayoutCount = pCreateInfo->setLayoutCount;
    pipelineLayout->setLayouts = ctsAllocation(
        pAllocator,
        sizeof(struct CtsDescriptorSetLayoutImpl) * pipelineLayout->setLayoutCount,
        alignof(struct CtsDescriptorSetLayoutImpl),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    for (uint32_t i = 0; i < pipelineLayout->setLayoutCount; ++i) {
        pipelineLayout->setLayouts[i] = pCreateInfo->pSetLayouts[i];
    }

    pipelineLayout->pushConstantRangeCount = pCreateInfo->pushConstantRangeCount;
    pipelineLayout->pushConstantRanges = ctsAllocation(
        pAllocator,
        sizeof(struct CtsPushConstantRange) * pipelineLayout->pushConstantRangeCount,
        alignof(struct CtsPushConstantRange),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    for (uint32_t i = 0; i < pipelineLayout->pushConstantRangeCount; ++i) {
        pipelineLayout->pushConstantRanges[i] = pCreateInfo->pPushConstantRanges[i];
    }

    *pPipelineLayout = pipelineLayout;
    return CTS_SUCCESS;
}

void ctsDestroyPipelineLayout(
    CtsDevice device,
    CtsPipelineLayout pipelineLayout,
    const CtsAllocationCallbacks* pAllocator
) {
    (void) device;

    if (pipelineLayout != NULL) {
        ctsFree(pAllocator, pipelineLayout->setLayouts);
        ctsFree(pAllocator, pipelineLayout->pushConstantRanges);
        ctsFree(pAllocator, pipelineLayout);
    }
}

#ifdef __cplusplus
}
#endif