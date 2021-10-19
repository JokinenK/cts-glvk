#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

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

CTS_DEFINE_TRAMPOLINE_VOID(
    CtsCmdPipelineBarrier, 
    ctsCmdPipelineBarrierImpl,
    commandBuffer,
    srcStageMask,
    dstStageMask,
    dependencyFlags,
    memoryBarrierCount,
    pMemoryBarriers,
    bufferMemoryBarrierCount,
    pBufferMemoryBarriers,
    imageMemoryBarrierCount,
    pImageMemoryBarriers
);

#ifdef __cplusplus
}
#endif