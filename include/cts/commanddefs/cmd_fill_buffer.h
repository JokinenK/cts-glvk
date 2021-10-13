#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdFillBuffer {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkBuffer dstBuffer;
    VkDeviceSize dstOffset;
    VkDeviceSize size;
    uint32_t data;
} CtsCmdFillBuffer;

#ifdef __cplusplus
}
#endif