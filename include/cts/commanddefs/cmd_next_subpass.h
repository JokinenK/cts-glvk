#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdNextSubpass {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkSubpassContents contents;
} CtsCmdNextSubpass;

CTS_DEFINE_COMMAND_TRAMPOLINE_VOID(
    CtsCmdNextSubpass, 
    ctsCmdNextSubpassImpl, 
    commandBuffer,
    contents
);

CTS_DEFINE_COMMAND_METADATA(CtsCmdNextSubpass);

#ifdef __cplusplus
}
#endif