#pragma once

#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsAllocateDescriptorSets(
    CtsDevice device,
    const CtsDescriptorSetAllocateInfo* pAllocateInfo,
    CtsDescriptorSet* pDescriptorSets
);

void ctsUpdateDescriptorSets(
    CtsDevice device,
    uint32_t descriptorWriteCount,
    const CtsWriteDescriptorSet* pDescriptorWrites,
    uint32_t descriptorCopyCount,
    const CtsCopyDescriptorSet* pDescriptorCopies
);

CtsResult ctsFreeDescriptorSets(
    CtsDevice device,
    CtsDescriptorPool descriptorPool,
    uint32_t descriptorSetCount,
    const CtsDescriptorSet* pDescriptorSets
);

#ifdef __cplusplus
}
#endif