#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdUpdateBuffer {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkBuffer dstBuffer;
    VkDeviceSize dstOffset;
    VkDeviceSize dataSize;
    const void* pData;
} CtsCmdUpdateBuffer;

CTS_DEFINE_TRAMPOLINE_VOID(
    CmdUpdateBuffer, 
    commandBuffer,
    dstBuffer,
    dstOffset,
    dataSize,
    pData
);

#ifdef __cplusplus
}
#endif