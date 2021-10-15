#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

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

CTS_DEFINE_TRAMPOLINE_VOID(
    CmdBindDescriptorSets, 
    commandBuffer, 
    pipelineBindPoint, 
    pipelineLayout,
    firstSet,
    descriptorSetCount,
    pDescriptorSets,
    dynamicOffsetCount,
    pDynamicOffsets
);

#ifdef __cplusplus
}
#endif