#pragma once

#include <stdint.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateGraphicsPipelines(
    CtsDevice pDevice,
    CtsPipelineCache pPipelineCache,
    uint32_t pCreateInfoCount,
    const CtsGraphicsPipelineCreateInfo* pCreateInfos,
    const CtsAllocationCallbacks* pAllocator,
    CtsPipeline* pPipelines
);

void ctsDestroyPipeline(
    CtsDevice pDevice,
    CtsPipeline pPipeline,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif