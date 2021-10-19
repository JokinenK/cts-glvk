#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdClearAttachments {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    uint32_t attachmentCount;
    const VkClearAttachment* pAttachments;
    uint32_t rectCount;
    const VkClearRect* pRects;
} CtsCmdClearAttachments;

CTS_DEFINE_COMMAND_TRAMPOLINE_VOID(
    CtsCmdClearAttachments, 
    ctsCmdClearAttachmentsImpl, 
    commandBuffer, 
    attachmentCount, 
    pAttachments,
    rectCount,
    pRects
);

CTS_DEFINE_COMMAND_METADATA(CtsCmdClearAttachments);

#ifdef __cplusplus
}
#endif