#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdResetQueryPool {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    CtsQueryPool queryPool;
    uint32_t firstQuery;
    uint32_t queryCount;
} CtsCmdResetQueryPool;

#ifdef __cplusplus
}
#endif