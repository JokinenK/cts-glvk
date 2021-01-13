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
    const CtsMemoryBarrier* pMemoryBarriers;
    uint32_t bufferMemoryBarrierCount;
    const CtsBufferMemoryBarrier* pBufferMemoryBarriers;
    uint32_t imageMemoryBarrierCount;
    const CtsImageMemoryBarrier* pImageMemoryBarriers;
} CtsCmdPipelineBarrier;

#ifdef __cplusplus
}
#endif