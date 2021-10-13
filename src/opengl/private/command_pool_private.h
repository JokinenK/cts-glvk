#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/allocator.h"
#include "cts/util/linear_allocator.h"
#include "cts/util/pool_allocator.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsCommandPool {
    CtsPoolAllocator poolAllocator;
    VkAllocationCallbacks allocator;
};

#ifdef __cplusplus
}
#endif