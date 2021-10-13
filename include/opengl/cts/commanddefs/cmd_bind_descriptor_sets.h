#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdBindDescriptorSets {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkPipelineBindPoint pipelineBindPoint;
    VkPipelineLayout pipelineLayout;
    uint32_t firstSet;
    uint32_t descriptorSetCount;
    const VkDescriptorSet* pDescriptorSets;
    uint32_t dynamicOffsetCount;
    const uint32_t* pDynamicOffsets;
} CtsCmdBindDescriptorSets;

#ifdef __cplusplus
}
#endif