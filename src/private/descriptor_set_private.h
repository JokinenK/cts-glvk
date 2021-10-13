#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/gl_enums.h"
#include "cts/gl_descriptor.h"
#include "private/object_base.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsDescriptorSet {
    struct CtsObjectBase base;
    struct CtsDescriptorSetLayout* layout;
    uint32_t descriptorCount;
    CtsGlDescriptor* descriptors;
};

VkResult VKAPI_CALL ctsAllocateDescriptorSetsImpl(
    VkDevice device,
    const VkDescriptorSetAllocateInfo* pAllocateInfo,
    VkDescriptorSet* pDescriptorSets
);

void ctsUpdateDescriptorSetsImpl(
    VkDevice device,
    uint32_t descriptorWriteCount,
    const VkWriteDescriptorSet* pDescriptorWrites,
    uint32_t descriptorCopyCount,
    const VkCopyDescriptorSet* pDescriptorCopies
);

VkResult ctsFreeDescriptorSetsImpl(
    VkDevice device,
    VkDescriptorPool descriptorPool,
    uint32_t descriptorSetCount,
    const VkDescriptorSet* pDescriptorSets
);

#ifdef __cplusplus
}
#endif