#pragma once

#include <cts/typedefs/buffer_view.h>
#include <cts/typedefs/descriptor_set.h>
#include <cts/typedefs/descriptor.h>
#include <cts/typedefs/gl_enums.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsDescriptorSet {
    CtsDescriptorSetLayout layout;
    uint32_t descriptorCount;
    CtsDescriptor* descriptors;
};

CtsResult ctsAllocateDescriptorSetsImpl(
    CtsDevice device,
    const CtsDescriptorSetAllocateInfo* pAllocateInfo,
    CtsDescriptorSet* pDescriptorSets
);

void ctsUpdateDescriptorSetsImpl(
    CtsDevice device,
    uint32_t descriptorWriteCount,
    const CtsWriteDescriptorSet* pDescriptorWrites,
    uint32_t descriptorCopyCount,
    const CtsCopyDescriptorSet* pDescriptorCopies
);

CtsResult ctsFreeDescriptorSetsImpl(
    CtsDevice device,
    CtsDescriptorPool descriptorPool,
    uint32_t descriptorSetCount,
    const CtsDescriptorSet* pDescriptorSets
);

#ifdef __cplusplus
}
#endif