#pragma once

#include <stdint.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateRenderPass(
    CtsDevice device,
    const CtsRenderPassCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsRenderPass* pRenderPass
);

void ctsDestroyRenderPass(
    CtsDevice device,
    CtsRenderPass renderPass,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif