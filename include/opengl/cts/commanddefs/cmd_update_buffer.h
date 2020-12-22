#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdUpdateBuffer {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    CtsBuffer dstBuffer;
    CtsDeviceSize dstOffset;
    CtsDeviceSize dataSize;
    const void* data;
} CtsCmdUpdateBuffer;

#ifdef __cplusplus
}
#endif