#pragma once

#include <cts/pool_allocator.h>
#include <cts/typedefs/query_pool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsQueryPoolImpl {
    CtsPoolAllocator poolAllocatorInstance;
    CtsAllocationCallbacks poolAllocator;
};

#ifdef __cplusplus
}
#endif