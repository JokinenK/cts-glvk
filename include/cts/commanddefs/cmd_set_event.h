#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdSetEvent {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkEvent event;
    VkPipelineStageFlags stageMask;
} CtsCmdSetEvent;

CTS_DEFINE_COMMAND_TRAMPOLINE_VOID(
    CtsCmdSetEvent, 
    ctsCmdSetEventImpl, 
    commandBuffer,
    event,
    stageMask
);

CTS_DEFINE_COMMAND_METADATA(CtsCmdSetEvent);

#ifdef __cplusplus
}
#endif