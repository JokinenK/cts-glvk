#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdSetDepthBounds {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    float minDepthBounds;
    float maxDepthBounds;
} CtsCmdSetDepthBounds;

CTS_DEFINE_TRAMPOLINE_VOID(
    CtsCmdSetDepthBounds, 
    ctsCmdSetDepthBoundsImpl, 
    commandBuffer,
    minDepthBounds,
    maxDepthBounds
);

#ifdef __cplusplus
}
#endif