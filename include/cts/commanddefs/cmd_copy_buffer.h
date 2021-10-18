#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdCopyBuffer {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkBuffer srcBuffer;
    VkBuffer dstBuffer;
    uint32_t regionCount;
    const VkBufferCopy* pRegions;
} CtsCmdCopyBuffer;

CTS_DEFINE_TRAMPOLINE_VOID(
    CmdCopyBuffer, 
    commandBuffer, 
    srcBuffer, 
    dstBuffer,
    regionCount,
    pRegions
);

#ifdef __cplusplus
}
#endif