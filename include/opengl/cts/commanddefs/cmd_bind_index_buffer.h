#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdBindIndexBuffer {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    CtsBuffer buffer;
    CtsDeviceSize offset;
    CtsIndexType indexType;
} CtsCmdBindIndexBuffer;

#ifdef __cplusplus
}
#endif