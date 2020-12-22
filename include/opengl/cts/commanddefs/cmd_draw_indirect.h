#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdDrawIndirect {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    CtsBuffer buffer;
    CtsDeviceSize offset;
    uint32_t drawCount;
    uint32_t stride;
} CtsCmdDrawIndirect;

#ifdef __cplusplus
}
#endif