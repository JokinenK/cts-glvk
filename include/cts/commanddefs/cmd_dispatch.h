#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdDispatch {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    uint32_t groupCountX;
    uint32_t groupCountY;
    uint32_t groupCountZ;
} CtsCmdDispatch;

CTS_DEFINE_TRAMPOLINE_VOID(
    CtsCmdDispatch, 
    ctsCmdDispatchImpl,
    commandBuffer, 
    groupCountX, 
    groupCountY,
    groupCountZ
);

#ifdef __cplusplus
}
#endif