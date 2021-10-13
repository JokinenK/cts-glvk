#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/allocator.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsAllocateMemory {
    CtsCmdBase base;
    VkDevice device;
    const VkMemoryAllocateInfo* pAllocateInfo;
    const VkAllocationCallbacks* pAllocator;
    VkDeviceMemory* pMemory;
    VkResult* pResult;
} CtsAllocateMemory;

#ifdef __cplusplus
}
#endif