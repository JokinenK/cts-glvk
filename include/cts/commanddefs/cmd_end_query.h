#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdEndQuery {
    CtsCmdBase base;
    VkCommandBuffer commandBuffer;
    VkQueryPool queryPool;
    uint32_t query;
} CtsCmdEndQuery;

CTS_DEFINE_COMMAND_TRAMPOLINE_VOID(
    CtsCmdEndQuery, 
    ctsCmdEndQueryImpl, 
    commandBuffer, 
    queryPool, 
    query
);

CTS_DEFINE_COMMAND_METADATA(CtsCmdEndQuery);

#ifdef __cplusplus
}
#endif