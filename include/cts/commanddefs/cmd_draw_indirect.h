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

CTS_DEFINE_TRAMPOLINE_VOID(
    CmdDrawIndirect, 
    commandBuffer, 
    buffer, 
    offset,
    drawCount,
    stride
);

#ifdef __cplusplus
}
#endif