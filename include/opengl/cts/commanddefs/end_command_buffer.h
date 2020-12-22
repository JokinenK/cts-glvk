#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsEndCommandBuffer {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    CtsResult* result;
} CtsEndCommandBuffer;

#ifdef __cplusplus
}
#endif