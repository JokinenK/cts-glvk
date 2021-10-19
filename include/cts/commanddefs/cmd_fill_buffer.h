#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

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

CTS_DEFINE_TRAMPOLINE_VOID(
    CtsCmdFillBuffer, 
    ctsCmdFillBufferImpl, 
    commandBuffer,
    dstBuffer,
    dstOffset,
    size,
    data
);

#ifdef __cplusplus
}
#endif