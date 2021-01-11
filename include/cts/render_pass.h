#pragma once

#include <stdint.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateRenderPass(
    CtsDevice pDevice,
    const CtsRenderPassCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsRenderPass* pRenderPass
);

void ctsDestroyRenderPass(
    CtsDevice pDevice,
    CtsRenderPass pRenderPass,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif