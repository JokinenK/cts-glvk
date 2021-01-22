#pragma once

#include <stdint.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateCommandPool(
    CtsDevice device,
    const CtsCommandPoolCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsCommandPool* pCommandPool
);

void ctsDestroyCommandPool(
    CtsDevice device,
    CtsCommandPool commandPool,
    const CtsAllocationCallbacks* pAllocator
);

CtsResult ctsAllocateCommandBuffers(
    CtsDevice device,
    const CtsCommandBufferAllocateInfo* pAllocateInfo,
    CtsCommandBuffer* pCommandBuffers
);

void ctsFreeCommandBuffers(
    CtsDevice device,
    CtsCommandPool commandPool,
    uint32_t commandBufferCount,
    const CtsCommandBuffer* pCommandBuffers
);

#ifdef __cplusplus
}
#endif