#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdSetStencilCompareMask {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkStencilFaceFlags faceMask;
    uint32_t compareMask;
} CtsCmdSetStencilCompareMask;

CTS_DEFINE_TRAMPOLINE_VOID(
    CmdSetStencilCompareMask, 
    commandBuffer,
    faceMask,
    compareMask
);

#ifdef __cplusplus
}
#endif