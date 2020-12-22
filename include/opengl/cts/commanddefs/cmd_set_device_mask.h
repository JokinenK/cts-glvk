#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdSetDeviceMask {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    uint32_t deviceMask;
} CtsCmdSetDeviceMask;

#ifdef __cplusplus
}
#endif