#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdWriteTimestamp {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    CtsPipelineStageFlagBits pipelineStage;
    CtsQueryPool queryPool;
    uint32_t query;
} CtsCmdWriteTimestamp;

#ifdef __cplusplus
}
#endif