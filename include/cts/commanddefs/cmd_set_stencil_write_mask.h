#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdSetStencilWriteMask {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkStencilFaceFlags faceMask;
    uint32_t writeMask;
} CtsCmdSetStencilWriteMask;

CTS_DEFINE_TRAMPOLINE_VOID(
    CmdSetStencilWriteMask, 
    commandBuffer,
    faceMask,
    writeMask
);

#ifdef __cplusplus
}
#endif