#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/typedefs/gl_enums.h"
#include "cts/typedefs/gl_descriptor.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsDescriptorSet {
    struct CtsDescriptorSetLayout* layout;
    uint32_t descriptorCount;
    CtsGlDescriptor* descriptors;
};

VkResult ctsAllocateDescriptorSetsImpl(
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