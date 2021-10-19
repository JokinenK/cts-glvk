#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdSetDeviceMask {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    uint32_t deviceMask;
} CtsCmdSetDeviceMask;

CTS_DEFINE_COMMAND_TRAMPOLINE_VOID(
    CtsCmdSetDeviceMask, 
    ctsCmdSetDeviceMaskImpl, 
    commandBuffer,
    deviceMask
);

CTS_DEFINE_COMMAND_METADATA(CtsCmdSetDeviceMask);

#ifdef __cplusplus
}
#endif