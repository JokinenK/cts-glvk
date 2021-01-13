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
    CtsDescriptorPool descriptorPool
);

#ifdef __cplusplus
}
#endif