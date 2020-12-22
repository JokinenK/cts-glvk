#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdCopyBuffer {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    CtsBuffer srcBuffer;
    CtsBuffer dstBuffer;
    uint32_t regionCount;
    const CtsBufferCopy* regions;
} CtsCmdCopyBuffer;

#ifdef __cplusplus
}
#endif