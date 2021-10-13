#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdSetEvent {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkEvent event;
    VkPipelineStageFlags stageMask;
} CtsCmdSetEvent;

#ifdef __cplusplus
}
#endif