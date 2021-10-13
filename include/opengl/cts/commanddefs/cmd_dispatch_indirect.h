#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdDispatchIndirect {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkBuffer buffer;
    VkDeviceSize offset;
} CtsCmdDispatchIndirect;

#ifdef __cplusplus
}
#endif