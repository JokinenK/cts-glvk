#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdBindPipeline {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    CtsPipelineBindPoint pipelineBindPoint;
    CtsPipeline pipeline;
} CtsCmdBindPipeline;

#ifdef __cplusplus
}
#endif