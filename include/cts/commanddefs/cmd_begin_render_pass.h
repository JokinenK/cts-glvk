#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdBeginRenderPass {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkRenderPassBeginInfo* pRenderPassBegin;
    VkSubpassContents contents;
} CtsCmdBeginRenderPass;

CTS_DEFINE_COMMAND_TRAMPOLINE_VOID(
    CtsCmdBeginRenderPass, 
    ctsCmdBeginRenderPassImpl, 
    commandBuffer, 
    pRenderPassBegin, 
    contents
);

CTS_DEFINE_COMMAND_METADATA(CtsCmdBeginRenderPass);

#ifdef __cplusplus
}
#endif