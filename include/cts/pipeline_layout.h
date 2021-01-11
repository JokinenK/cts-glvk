#pragma once

#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreatePipelineLayout(
    CtsDevice pDevice,
    const CtsPipelineLayoutCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsPipelineLayout* pPipelineLayout
);

void ctsDestroyPipelineLayout(
    CtsDevice pDevice,
    CtsPipelineLayout pPipelineLayout,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif