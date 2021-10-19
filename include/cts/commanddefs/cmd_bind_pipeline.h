#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdBindPipeline {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkPipelineBindPoint pipelineBindPoint;
    VkPipeline pipeline;
} CtsCmdBindPipeline;

CTS_DEFINE_COMMAND_TRAMPOLINE_VOID(
    CtsCmdBindPipeline, 
    ctsCmdBindPipelineImpl, 
    commandBuffer, 
    pipelineBindPoint, 
    pipeline
);

CTS_DEFINE_COMMAND_METADATA(CtsCmdBindPipeline);

#ifdef __cplusplus
}
#endif