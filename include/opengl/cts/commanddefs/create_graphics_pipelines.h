#pragma once

#include <stdint.h>
#include <cts/allocator.h>
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
    const CtsGraphicsPipelineCreateInfo* pCreateInfos;
    const CtsAllocationCallbacks* pAllocator;
    CtsPipeline* pPipelines;
    CtsResult* pResult;
} CtsCreateGraphicsPipelines;

#ifdef __cplusplus
}
#endif