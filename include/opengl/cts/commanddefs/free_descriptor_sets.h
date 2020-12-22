#pragma once

#include <stdint.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsFreeDescriptorSets {
    CtsCmdBase base;
    CtsDevice device;
    CtsDescriptorPool descriptorPool;
    uint32_t descriptorSetCount;
    const CtsDescriptorSet* descriptorSets;
    CtsResult* result;
} CtsFreeDescriptorSets;

#ifdef __cplusplus
}
#endif