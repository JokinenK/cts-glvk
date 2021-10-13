#include <stddef.h>
#include <cts/descriptor_set_layout.h>
#include <cts/allocator.h>
#include <cts/util/align.h>
#include <private/private.h>
#include <private/descriptor_set_layout_private.h>

#ifdef __cplusplus
extern "C" {
#endif

VkResult ctsCreateDescriptorSetLayout(
    VkDevice device,
    const VkDescriptorSetLayoutCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDescriptorSetLayout* pDescriptorSetLayout
) {
    (void) device;

    struct CtsDescriptorSetLayout* descriptorSetLayout = ctsAllocation(
        pAllocator,
        sizeof(struct CtsDescriptorSetLayout),
        alignof(struct CtsDescriptorSetLayout),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (descriptorSetLayout == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    descriptorSetLayout->bindingCount = pCreateInfo->bindingCount;
    descriptorSetLayout->bindings = ctsAllocation(
        pAllocator,
        sizeof(struct CtsGlDescriptorSetLayoutBinding) * descriptorSetLayout->bindingCount,
        alignof(struct CtsGlDescriptorSetLayoutBinding),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    uint32_t descriptorOffset = 0;
    for (uint32_t i = 0; i < descriptorSetLayout->bindingCount; ++i) {
        struct CtsGlDescriptorSetLayoutBinding* target = &descriptorSetLayout->bindings[i];
        const VkDescriptorSetLayoutBinding* source = &pCreateInfo->pBindings[i];

        target->binding = source->binding;
        target->descriptorType = source->descriptorType;
        target->descriptorCount = source->descriptorCount;
        target->descriptorOffset = descriptorOffset;
        target->stageFlags = source->stageFlags;
        target->immutableSamplers = source->pImmutableSamplers;

        descriptorOffset += source->descriptorCount;
    }
    
    *pDescriptorSetLayout = CtsDescriptorSetLayoutToHandle(descriptorSetLayout);
    return VK_SUCCESS;
}

void ctsDestroyDescriptorSetLayout(
    VkDevice device,
    VkDescriptorSetLayout descriptorSetLayoutHandle,
    const VkAllocationCallbacks* pAllocator
) {
    (void) device;

    struct CtsDescriptorSetLayout* descriptorSetLayout = CtsDescriptorSetLayoutFromHandle(descriptorSetLayoutHandle);

    if (descriptorSetLayout != NULL) {
        ctsFree(pAllocator, descriptorSetLayout->bindings);
        ctsFree(pAllocator, descriptorSetLayout);
    }
}

#ifdef __cplusplus
}
#endif