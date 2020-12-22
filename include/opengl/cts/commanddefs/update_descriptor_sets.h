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
    const CtsWriteDescriptorSet* descriptorWrites;
    uint32_t descriptorCopyCount;
    const CtsCopyDescriptorSet* descriptorCopies;
} CtsUpdateDescriptorSets;

#ifdef __cplusplus
}
#endif
