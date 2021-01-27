#pragma once

#include <stdint.h>
#include <cts/allocator.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateGraphicsPipelines(
    CtsDevice device,
    CtsPipelineCache pipelineCache,
    uint32_t createInfoCount,
    const CtsGraphicsPipelineCreateInfo* pCreateInfos,
    const CtsAllocationCallbacks* pAllocator,
    CtsPipeline* pPipelines
);

void ctsDestroyPipeline(
    CtsDevice device,
    CtsPipeline pipeline,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif