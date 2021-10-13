#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdPipelineBarrier {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkPipelineStageFlags srcStageMask;
    VkPipelineStageFlags dstStageMask;
    VkDependencyFlags dependencyFlags;
    uint32_t memoryBarrierCount;
    VkMemoryBarrier* pMemoryBarriers;
    uint32_t bufferMemoryBarrierCount;
    VkBufferMemoryBarrier* pBufferMemoryBarriers;
    uint32_t imageMemoryBarrierCount;
    VkImageMemoryBarrier* pImageMemoryBarriers;
} CtsCmdPipelineBarrier;

#ifdef __cplusplus
}
#endif