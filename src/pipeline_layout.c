#include <stddef.h>
#include <cts/pipeline_layout.h>
#include <cts/allocator.h>
#include <cts/util/align.h>
#include <private/private.h>
#include <private/pipeline_layout_private.h>
#include <private/descriptor_set_layout_private.h>

#ifdef __cplusplus
extern "C" {
#endif

VkResult VKAPI_CALL ctsCreatePipelineLayout(
    VkDevice deviceHandle,
    const VkPipelineLayoutCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkPipelineLayout* pPipelineLayout
) {
    (void) deviceHandle;

    struct CtsPipelineLayout* pipelineLayout = ctsAllocation(
        pAllocator,
        sizeof(struct CtsPipelineLayout),
        alignof(struct CtsPipelineLayout),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (pipelineLayout == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    set_loader_magic_value(pipelineLayout);

    pipelineLayout->setLayoutCount = pCreateInfo->setLayoutCount;
    pipelineLayout->ppSetLayouts = ctsAllocation(
        pAllocator,
        sizeof(struct CtsDescriptorSetLayout*) * pipelineLayout->setLayoutCount,
        alignof(struct CtsDescriptorSetLayout*),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    for (uint32_t i = 0; i < pipelineLayout->setLayoutCount; ++i) {
        pipelineLayout->ppSetLayouts[i] = CtsDescriptorSetLayoutFromHandle(pCreateInfo->pSetLayouts[i]);
    }

    pipelineLayout->pushConstantRangeCount = pCreateInfo->pushConstantRangeCount;
    pipelineLayout->pushConstantRanges = ctsAllocation(
        pAllocator,
        sizeof(VkPushConstantRange) * pipelineLayout->pushConstantRangeCount,
        alignof(VkPushConstantRange),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    for (uint32_t i = 0; i < pipelineLayout->pushConstantRangeCount; ++i) {
        pipelineLayout->pushConstantRanges[i] = pCreateInfo->pPushConstantRanges[i];
    }

    *pPipelineLayout = CtsPipelineLayoutToHandle(pipelineLayout);
    return VK_SUCCESS;
}

void VKAPI_CALL ctsDestroyPipelineLayout(
    VkDevice deviceHandle,
    VkPipelineLayout pipelineLayoutHandle,
    const VkAllocationCallbacks* pAllocator
) {
    (void) deviceHandle;

    struct CtsPipelineLayout* pipelineLayout = CtsPipelineLayoutFromHandle(pipelineLayoutHandle);

    if (pipelineLayout != NULL) {
        ctsFree(pAllocator, pipelineLayout->ppSetLayouts);
        ctsFree(pAllocator, pipelineLayout->pushConstantRanges);
        ctsFree(pAllocator, pipelineLayout);
    }
}

#ifdef __cplusplus
}
#endif