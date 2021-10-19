#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdResetQueryPool {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkQueryPool queryPool;
    uint32_t firstQuery;
    uint32_t queryCount;
} CtsCmdResetQueryPool;

CTS_DEFINE_COMMAND_TRAMPOLINE_VOID(
    CtsCmdResetQueryPool, 
    ctsCmdResetQueryPoolImpl, 
    commandBuffer,
    queryPool,
    firstQuery,
    queryCount
);

CTS_DEFINE_COMMAND_METADATA(CtsCmdResetQueryPool);

#ifdef __cplusplus
}
#endif