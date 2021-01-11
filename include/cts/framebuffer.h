#pragma once

#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateFramebuffer(
    CtsDevice pDevice,
    const CtsFramebufferCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsFramebuffer* pFramebuffer
);

void ctsDestroyFramebuffer(
    CtsDevice pDevice,
    CtsFramebuffer pFramebuffer,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif