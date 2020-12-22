#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdCopyQueryPoolResults {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    CtsQueryPool queryPool;
    uint32_t firstQuery;
    uint32_t queryCount;
    CtsBuffer dstBuffer;
    CtsDeviceSize dstOffset;
    CtsDeviceSize stride;
    CtsQueryResultFlags flags;
} CtsCmdCopyQueryPoolResults;

#ifdef __cplusplus
}
#endif