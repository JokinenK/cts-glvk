#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdPipelineBarrier {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    CtsPipelineStageFlags srcStageMask;
    CtsPipelineStageFlags dstStageMask;
    CtsDependencyFlags dependencyFlags;
    uint32_t memoryBarrierCount;
    CtsMemoryBarrier* pMemoryBarriers;
    uint32_t bufferMemoryBarrierCount;
    CtsBufferMemoryBarrier* pBufferMemoryBarriers;
    uint32_t imageMemoryBarrierCount;
    CtsImageMemoryBarrier* pImageMemoryBarriers;
} CtsCmdPipelineBarrier;

#ifdef __cplusplus
}
#endif