#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdDispatchIndirect {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    CtsBuffer buffer;
    CtsDeviceSize offset;
} CtsCmdDispatchIndirect;

#ifdef __cplusplus
}
#endif