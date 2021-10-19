#pragma once

#include <stdint.h>
#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/device_memory_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsUnmapMemory {
    CtsCmdBase base;
    VkDevice device;
    VkDeviceMemory memory;
} CtsUnmapMemory;

CTS_DEFINE_TRAMPOLINE_VOID(
    CtsUnmapMemory,
    ctsUnmapMemoryImpl,
    device,
    memory
);

#ifdef __cplusplus
}
#endif