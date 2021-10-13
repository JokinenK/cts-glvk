#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsFreeMemory {
    CtsCmdBase base;
    VkDevice device;
    VkDeviceMemory memory;
    const VkAllocationCallbacks* pAllocator;
} CtsFreeMemory;

#ifdef __cplusplus
}
#endif