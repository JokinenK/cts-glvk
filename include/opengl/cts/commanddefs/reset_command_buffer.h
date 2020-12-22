#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsResetCommandBuffer {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    CtsCommandBufferResetFlags flags;
    CtsResult* result;
} CtsResetCommandBuffer;

#ifdef __cplusplus
}
#endif