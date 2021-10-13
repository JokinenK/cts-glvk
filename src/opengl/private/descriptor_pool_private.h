#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/allocator.h"
#include "cts/util/linear_allocator.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsDescriptorPool {
    CtsLinearAllocator linearAllocator;
    VkAllocationCallbacks allocator; 
};

#ifdef __cplusplus
}
#endif