#pragma once

#include <cts/pool_allocator.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsQueryPool {
    CtsPoolAllocator poolAllocatorInstance;
    CtsAllocationCallbacks poolAllocator;
};

#ifdef __cplusplus
}
#endif