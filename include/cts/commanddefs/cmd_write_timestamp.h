#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdWriteTimestamp {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkPipelineStageFlagBits pipelineStage;
    VkQueryPool queryPool;
    uint32_t query;
} CtsCmdWriteTimestamp;

CTS_DEFINE_TRAMPOLINE_VOID(
    CmdWriteTimestamp, 
    commandBuffer,
    pipelineStage,
    queryPool,
    query
);

#ifdef __cplusplus
}
#endif