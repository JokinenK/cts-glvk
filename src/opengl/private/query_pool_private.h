#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/util/pool_allocator.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsQueryPool {
    CtsPoolAllocator poolAllocatorInstance;
    VkAllocationCallbacks poolAllocator;
};

#ifdef __cplusplus
}
#endif