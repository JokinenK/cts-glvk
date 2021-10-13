#pragma once

#include <stdint.h>
#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsUnmapMemory {
    CtsCmdBase base;
    VkDevice device;
    VkDeviceMemory memory;
} CtsUnmapMemory;

#ifdef __cplusplus
}
#endif