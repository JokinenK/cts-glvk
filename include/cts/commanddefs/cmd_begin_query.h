#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdBeginQuery {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkQueryPool queryPool;
    uint32_t query;
    VkQueryControlFlags flags;
} CtsCmdBeginQuery;

CTS_DEFINE_COMMAND_TRAMPOLINE_VOID(
    CtsCmdBeginQuery, 
    ctsCmdBeginQueryImpl, 
    commandBuffer, 
    queryPool, 
    query,
    flags
);

CTS_DEFINE_COMMAND_METADATA(CtsCmdBeginQuery);

#ifdef __cplusplus
}
#endif