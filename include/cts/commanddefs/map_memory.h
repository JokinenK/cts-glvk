#pragma once

#include <stdint.h>
#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/device_memory_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsMapMemory {
    CtsCmdBase base;
    VkDevice device;
    VkDeviceMemory memory;
    VkDeviceSize offset;
    VkDeviceSize size;
    VkMemoryMapFlags flags;
    void** ppData;
    VkResult* pResult;
} CtsMapMemory;

CTS_DEFINE_TRAMPOLINE(
    CtsMapMemory,
    ctsMapMemoryImpl,
    device,
    memory,
    offset,
    size,
    flags,
    ppData
);

#ifdef __cplusplus
}
#endif