#pragma once

#include <stdint.h>
#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

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

#ifdef __cplusplus
}
#endif