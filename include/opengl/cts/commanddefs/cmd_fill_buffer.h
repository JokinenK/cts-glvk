#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdFillBuffer {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    CtsBuffer dstBuffer;
    CtsDeviceSize dstOffset;
    CtsDeviceSize size;
    uint32_t data;
} CtsCmdFillBuffer;

#ifdef __cplusplus
}
#endif