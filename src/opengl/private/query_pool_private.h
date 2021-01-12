#pragma once

#include <cts/bump_allocator.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsQueryPool {
    CtsBumpAllocator bumpAllocatorInstance;
    CtsAllocationCallbacks bumpAllocator;
};

#ifdef __cplusplus
}
#endif