#pragma once

#include <cts/allocator.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateFramebuffer(
    CtsDevice device,
    const CtsFramebufferCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsFramebuffer* pFramebuffer
);

void ctsDestroyFramebuffer(
    CtsDevice device,
    CtsFramebuffer framebuffer,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif