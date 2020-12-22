#pragma once

#include <cts/allocator.h>
#include <cts/bump_allocator.h>

#ifdef __cplusplus
extern "C" {
#endif

void ctsInitBumpAllocatorProxy(CtsBumpAllocator pInstance, CtsAllocationCallbacks* pAllocator);

#ifdef __cplusplus
}
#endif