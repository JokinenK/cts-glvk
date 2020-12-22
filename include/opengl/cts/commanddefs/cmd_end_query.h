#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdEndQuery {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    CtsQueryPool queryPool;
    uint32_t query;
} CtsCmdEndQuery;

#ifdef __cplusplus
}
#endif