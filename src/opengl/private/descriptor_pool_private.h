#pragma once

#include <cts/allocator.h>
#include <cts/linear_allocator.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsDescriptorPool {
    CtsLinearAllocator linearAllocatorInstance;
    CtsAllocationCallbacks linearAllocator;
};

CtsDescriptorSet ctsDescriptorPoolAllocateSet(
    CtsDescriptorPool pDescriptorPool
);

#ifdef __cplusplus
}
#endif