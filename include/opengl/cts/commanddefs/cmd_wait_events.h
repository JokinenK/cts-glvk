#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdWaitEvents {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    uint32_t eventCount;
    const CtsEvent* events;
    CtsPipelineStageFlags srcStageMask;
    CtsPipelineStageFlags dstStageMask;
    uint32_t memoryBarrierCount;
    const CtsMemoryBarrier* pMemoryBarriers;
    uint32_t bufferMemoryBarrierCount;
    const CtsBufferMemoryBarrier* pBufferMemoryBarriers;
    uint32_t imageMemoryBarrierCount;
    const CtsImageMemoryBarrier* pImageMemoryBarriers;
} CtsCmdWaitEvents;

#ifdef __cplusplus
}
#endif