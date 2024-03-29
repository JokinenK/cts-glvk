#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdSetStencilReference {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkStencilFaceFlags faceMask;
    uint32_t reference;
} CtsCmdSetStencilReference;

CTS_DEFINE_COMMAND_TRAMPOLINE_VOID(
    CtsCmdSetStencilReference, 
    ctsCmdSetStencilReferenceImpl, 
    commandBuffer,
    faceMask,
    reference
);

CTS_DEFINE_COMMAND_METADATA(CtsCmdSetStencilReference);

#ifdef __cplusplus
}
#endif