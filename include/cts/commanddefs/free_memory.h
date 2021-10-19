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

CTS_DEFINE_COMMAND_TRAMPOLINE_VOID(
    CtsFreeMemory,
    ctsFreeMemoryImpl,
    device,
    memory,
    pAllocator
);

CTS_DEFINE_COMMAND_METADATA(CtsFreeMemory);

#ifdef __cplusplus
}
#endif