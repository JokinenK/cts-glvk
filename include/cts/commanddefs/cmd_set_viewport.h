#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdSetViewport {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    uint32_t firstViewport;
    uint32_t viewportCount;
    const VkViewport* pViewports;
} CtsCmdSetViewport;

CTS_DEFINE_TRAMPOLINE_VOID(
    CtsCmdSetViewport, 
    ctsCmdSetViewportImpl, 
    commandBuffer,
    firstViewport,
    viewportCount,
    pViewports
);

#ifdef __cplusplus
}
#endif