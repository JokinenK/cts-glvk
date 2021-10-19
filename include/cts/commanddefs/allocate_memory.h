#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/allocator.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/device_memory_private.h"

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

CTS_DEFINE_COMMAND_TRAMPOLINE(
    CtsAllocateMemory, 
    ctsAllocateMemoryImpl, 
    device, 
    pAllocateInfo, 
    pAllocator,
    pMemory
);

CTS_DEFINE_COMMAND_METADATA(CtsAllocateMemory);

#ifdef __cplusplus
}
#endif