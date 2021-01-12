#pragma once

#include <cts/allocator.h>
#include <cts/pool_allocator.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsCommandPool {
    CtsPoolAllocator poolAllocatorInstance;
    CtsAllocationCallbacks poolAllocator;
};

#ifdef __cplusplus
}
#endif