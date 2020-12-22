#pragma once

#include <cts/allocator.h>
#include <cts/bump_allocator.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsCommandPool {
    CtsBumpAllocator bumpAllocatorInstance;
    CtsAllocationCallbacks bumpAllocator;
};

#ifdef __cplusplus
}
#endif