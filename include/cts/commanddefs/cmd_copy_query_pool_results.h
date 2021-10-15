#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdCopyQueryPoolResults {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkQueryPool queryPool;
    uint32_t firstQuery;
    uint32_t queryCount;
    VkBuffer dstBuffer;
    VkDeviceSize dstOffset;
    VkDeviceSize stride;
    VkQueryResultFlags flags;
} CtsCmdCopyQueryPoolResults;

CTS_DEFINE_TRAMPOLINE_VOID(
    CmdCopyQueryPoolResults, 
    commandBuffer, 
    queryPool, 
    firstQuery,
    queryCount,
    dstBuffer,
    dstOffset,
    stride,
    flags
);

#ifdef __cplusplus
}
#endif