#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsBeginCommandBuffer {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    const CtsCommandBufferBeginInfo* beginInfo;
    CtsResult* result;
} CtsBeginCommandBuffer;

#ifdef __cplusplus
}
#endif