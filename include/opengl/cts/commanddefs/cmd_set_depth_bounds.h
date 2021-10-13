#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdSetDepthBounds {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    float minDepthBounds;
    float maxDepthBounds;
} CtsCmdSetDepthBounds;

#ifdef __cplusplus
}
#endif