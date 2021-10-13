#pragma once

#include <stdint.h>
#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

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

#ifdef __cplusplus
}
#endif