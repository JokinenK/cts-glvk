#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdBeginQuery {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    CtsQueryPool queryPool;
    uint32_t query;
    CtsQueryControlFlags flags;
} CtsCmdBeginQuery;

#ifdef __cplusplus
}
#endif