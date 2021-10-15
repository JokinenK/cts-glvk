#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdWaitEvents {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    uint32_t eventCount;
    const VkEvent* events;
    VkPipelineStageFlags srcStageMask;
    VkPipelineStageFlags dstStageMask;
    uint32_t memoryBarrierCount;
    const VkMemoryBarrier* pMemoryBarriers;
    uint32_t bufferMemoryBarrierCount;
    const VkBufferMemoryBarrier* pBufferMemoryBarriers;
    uint32_t imageMemoryBarrierCount;
    const VkImageMemoryBarrier* pImageMemoryBarriers;
} CtsCmdWaitEvents;

CTS_DEFINE_TRAMPOLINE_VOID(
    CmdWaitEvents, 
    commandBuffer,
    eventCount,
    events,
    srcStageMask,
    dstStageMask,
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