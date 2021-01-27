#pragma once

#include <cts/allocator.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreatePipelineLayout(
    CtsDevice device,
    const CtsPipelineLayoutCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsPipelineLayout* pPipelineLayout
);

void ctsDestroyPipelineLayout(
    CtsDevice device,
    CtsPipelineLayout pipelineLayout,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif