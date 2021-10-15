#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdBindIndexBuffer {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkBuffer buffer;
    VkDeviceSize offset;
    VkIndexType indexType;
} CtsCmdBindIndexBuffer;

CTS_DEFINE_TRAMPOLINE_VOID(
    CmdBindIndexBuffer, 
    commandBuffer, 
    buffer, 
    offset,
    indexType
);

#ifdef __cplusplus
}
#endif