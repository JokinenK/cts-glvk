#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsAllocateCommandBuffers {
    CtsCmdBase base;
    CtsDevice device;
    const CtsCommandBufferAllocateInfo* allocateInfo;
    CtsCommandBuffer* commandBuffers;
    CtsResult* result;
} CtsAllocateCommandBuffers;

#ifdef __cplusplus
}
#endif