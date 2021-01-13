#include <stddef.h>
#include <cts/descriptor_set_layout.h>
#include <private/descriptor_set_layout_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateDescriptorSetLayout(
    CtsDevice pDevice,
    const CtsDescriptorSetLayoutCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsDescriptorSetLayout* pDescriptorSetLayout
) {
    (void) pDevice;

    CtsDescriptorSetLayout descriptorSetLayout = ctsAllocation(
        pAllocator,
        sizeof(CtsDescriptorSetLayout),
        alignof(CtsDescriptorSetLayout),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (descriptorSetLayout == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    descriptorSetLayout->bindingCount = pCreateInfo->bindingCount;
    descriptorSetLayout->bindings = ctsAllocation(
        pAllocator,
        sizeof(CtsDescriptorSetLayoutBinding) * descriptorSetLayout->bindingCount,
        alignof(CtsDescriptorSetLayoutBinding),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    uint32_t descriptorOffset = 0;
    for (uint32_t i = 0; i < descriptorSetLayout->bindingCount; ++i) {
        CtsGlDescriptorSetLayoutBinding* target = &descriptorSetLayout->bindings[i];
        const CtsDescriptorSetLayoutBinding* source = &pCreateInfo->bindings[i];

        target->binding = source->binding;
        target->descriptorType = source->descriptorType;
        target->descriptorCount = source->descriptorCount;
        target->descriptorOffset = descriptorOffset;
        target->stageFlags = source->stageFlags;
        target->immutableSamplers = source->immutableSamplers;

        descriptorOffset += source->descriptorCount;
    }
    
    *pDescriptorSetLayout = descriptorSetLayout;
    return CTS_SUCCESS;
}

void ctsDestroyDescriptorSetLayout(
    CtsDevice pDevice,
    CtsDescriptorSetLayout pDescriptorSetLayout,
    const CtsAllocationCallbacks* pAllocator
) {
    (void) pDevice;

    if (pDescriptorSetLayout != NULL) {
        ctsFree(pAllocator, pDescriptorSetLayout->bindings);
        ctsFree(pAllocator, pDescriptorSetLayout);
    }
}

#ifdef __cplusplus
}
#endif