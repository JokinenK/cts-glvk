#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdDrawIndirect {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkBuffer buffer;
    VkDeviceSize offset;
    uint32_t drawCount;
    uint32_t stride;
} CtsCmdDrawIndirect;

CTS_DEFINE_COMMAND_TRAMPOLINE_VOID(
    CtsCmdDrawIndirect, 
    ctsCmdDrawIndirectImpl, 
    commandBuffer, 
    buffer, 
    offset,
    drawCount,
    stride
);

CTS_DEFINE_COMMAND_METADATA(CtsCmdDrawIndirect);

#ifdef __cplusplus
}
#endif