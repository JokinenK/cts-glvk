#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdResetEvent {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkEvent event;
    VkPipelineStageFlags stageMask;
} CtsCmdResetEvent;

CTS_DEFINE_COMMAND_TRAMPOLINE_VOID(
    CtsCmdResetEvent, 
    ctsCmdResetEventImpl, 
    commandBuffer,
    event,
    stageMask
);

CTS_DEFINE_COMMAND_METADATA(CtsCmdResetEvent);

#ifdef __cplusplus
}
#endif