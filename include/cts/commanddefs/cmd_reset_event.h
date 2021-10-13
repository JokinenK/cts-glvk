#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdResetEvent {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkEvent event;
    VkPipelineStageFlags stageMask;
} CtsCmdResetEvent;

#ifdef __cplusplus
}
#endif