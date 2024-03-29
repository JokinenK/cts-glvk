#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdEndRenderPass {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
} CtsCmdEndRenderPass;

CTS_DEFINE_COMMAND_TRAMPOLINE_VOID(
    CtsCmdEndRenderPass, 
    ctsCmdEndRenderPassImpl, 
    commandBuffer
);

CTS_DEFINE_COMMAND_METADATA(CtsCmdEndRenderPass);

#ifdef __cplusplus
}
#endif