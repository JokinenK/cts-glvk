#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdSetScissor {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    uint32_t firstScissor;
    uint32_t scissorCount;
    const VkRect2D* pScissors;
} CtsCmdSetScissor;

#ifdef __cplusplus
}
#endif