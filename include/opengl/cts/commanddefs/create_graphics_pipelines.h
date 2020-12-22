#pragma once

#include <stdint.h>
#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCreateGraphicsPipelines {
    CtsCmdBase base;
    CtsDevice device;
    CtsPipelineCache pipelineCache;
    uint32_t createInfoCount;
    const CtsGraphicsPipelineCreateInfo* createInfos;
    const CtsAllocationCallbacks* allocator;
    CtsPipeline* pipelines;
    CtsResult* result;
} CtsCreateGraphicsPipelines;

#ifdef __cplusplus
}
#endif