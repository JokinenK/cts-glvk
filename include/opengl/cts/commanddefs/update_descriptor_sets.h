#pragma once

#include <stdint.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsUpdateDescriptorSets {
    CtsCmdBase base;
    CtsDevice device;
    uint32_t descriptorWriteCount;
    const CtsWriteDescriptorSet* pDescriptorWrites;
    uint32_t descriptorCopyCount;
    const CtsCopyDescriptorSet* pDescriptorCopies;
} CtsUpdateDescriptorSets;

#ifdef __cplusplus
}
#endif
