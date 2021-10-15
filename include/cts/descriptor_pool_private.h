#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/allocator.h"
#include "cts/util/linear_allocator.h"
#include "cts/object_base.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsDescriptorPool {
    struct CtsObjectBase base;
    CtsLinearAllocator linearAllocator;
    VkAllocationCallbacks allocator; 
};

#ifdef __cplusplus
}
#endif