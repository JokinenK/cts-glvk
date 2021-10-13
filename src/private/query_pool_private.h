#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/util/pool_allocator.h"
#include "private/object_base.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsQueryPool {
    struct CtsObjectBase base;
    CtsPoolAllocator poolAllocatorInstance;
    VkAllocationCallbacks poolAllocator;
};

#ifdef __cplusplus
}
#endif