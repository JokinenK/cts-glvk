#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdDispatchIndirect {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkBuffer buffer;
    VkDeviceSize offset;
} CtsCmdDispatchIndirect;

CTS_DEFINE_TRAMPOLINE_VOID(
    CmdDispatchIndirect, 
    commandBuffer, 
    buffer, 
    offset
);

#ifdef __cplusplus
}
#endif