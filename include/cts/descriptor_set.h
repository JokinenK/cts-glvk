#pragma once

#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsAllocateDescriptorSets(
    CtsDevice pDevice,
    const CtsDescriptorSetAllocateInfo* pAllocateInfo,
    CtsDescriptorSet* pDescriptorSets
);

void ctsUpdateDescriptorSets(
    CtsDevice pDevice,
    uint32_t pDescriptorWriteCount,
    const CtsWriteDescriptorSet* pDescriptorWrites,
    uint32_t pDescriptorCopyCount,
    const CtsCopyDescriptorSet* pDescriptorCopies
);

CtsResult ctsFreeDescriptorSets(
    CtsDevice pDevice,
    CtsDescriptorPool pDescriptorPool,
    uint32_t pDescriptorSetCount,
    const CtsDescriptorSet* pDescriptorSets
);

#ifdef __cplusplus
}
#endif