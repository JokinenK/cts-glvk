#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdBindPipeline {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkPipelineBindPoint pipelineBindPoint;
    VkPipeline pipeline;
} CtsCmdBindPipeline;

#ifdef __cplusplus
}
#endif