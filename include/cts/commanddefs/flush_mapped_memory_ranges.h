#pragma once

#include <stdint.h>
#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/device_memory_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsFlushMappedMemoryRanges {
    CtsCmdBase base;
    VkDevice device;
    uint32_t memoryRangeCount;
    const VkMappedMemoryRange* pMemoryRanges;
    VkResult* pResult;
} CtsFlushMappedMemoryRanges;

CTS_DEFINE_COMMAND_TRAMPOLINE(
    CtsFlushMappedMemoryRanges,
    ctsFlushMappedMemoryRangesImpl,
    device,
    memoryRangeCount,
    pMemoryRanges
);

CTS_DEFINE_COMMAND_METADATA(CtsFlushMappedMemoryRanges);

#ifdef __cplusplus
}
#endif