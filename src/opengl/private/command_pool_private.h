#pragma once

#include <cts/allocator.h>
#include <cts/linear_allocator.h>
#include <cts/pool_allocator.h>
#include <cts/typedefs/command_pool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsCommandPoolImpl {
    CtsPoolAllocator poolAllocator;
    CtsAllocationCallbacks allocator;
};

#ifdef __cplusplus
}
#endif