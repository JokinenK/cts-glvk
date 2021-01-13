#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdBindDescriptorSets {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    CtsPipelineBindPoint pipelineBindPoint;
    CtsPipelineLayout pipelineLayout;
    uint32_t firstSet;
    uint32_t descriptorSetCount;
    const CtsDescriptorSet* pDescriptorSets;
    uint32_t dynamicOffsetCount;
    const uint32_t* pDynamicOffsets;
} CtsCmdBindDescriptorSets;

#ifdef __cplusplus
}
#endif