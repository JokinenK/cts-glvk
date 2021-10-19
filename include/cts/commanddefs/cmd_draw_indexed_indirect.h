#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdDrawIndexedIndirect {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkBuffer buffer;
    VkDeviceSize offset;
    uint32_t drawCount;
    uint32_t stride;
} CtsCmdDrawIndexedIndirect;

CTS_DEFINE_COMMAND_TRAMPOLINE_VOID(
    CtsCmdDrawIndexedIndirect, 
    ctsCmdDrawIndexedIndirectImpl,
    commandBuffer, 
    buffer, 
    offset,
    drawCount,
    stride
);

CTS_DEFINE_COMMAND_METADATA(CtsCmdDrawIndexedIndirect);

#ifdef __cplusplus
}
#endif