#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/device_memory_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsFreeMemory {
    CtsCmdBase base;
    VkDevice device;
    VkDeviceMemory memory;
    const VkAllocationCallbacks* pAllocator;
} CtsFreeMemory;

CTS_DEFINE_TRAMPOLINE_VOID(
    FreeMemory,
    device,
    memory,
    pAllocator
);

#ifdef __cplusplus
}
#endif