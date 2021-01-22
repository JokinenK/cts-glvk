#pragma once

#include <cts/allocator.h>
#include <cts/linear_allocator.h>
#include <cts/typedefs/descriptor_pool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsDescriptorPoolImpl {
    CtsLinearAllocator linearAllocator;
    CtsAllocationCallbacks allocator; 
};

#ifdef __cplusplus
}
#endif