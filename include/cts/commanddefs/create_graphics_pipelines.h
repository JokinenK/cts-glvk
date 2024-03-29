#pragma once

#include <stdint.h>
#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/pipeline_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCreateGraphicsPipelines {
    CtsCmdBase base;
    VkDevice device;
    VkPipelineCache pipelineCache;
    uint32_t createInfoCount;
    const VkGraphicsPipelineCreateInfo* pCreateInfos;
    const VkAllocationCallbacks* pAllocator;
    VkPipeline* pPipelines;
    VkResult* pResult;
} CtsCreateGraphicsPipelines;

CTS_DEFINE_COMMAND_TRAMPOLINE(
    CtsCreateGraphicsPipelines, 
    ctsCreateGraphicsPipelinesImpl, 
    device,
    pipelineCache,
    createInfoCount,
    pCreateInfos,
    pAllocator,
    pPipelines
);

CTS_DEFINE_COMMAND_METADATA(CtsCreateGraphicsPipelines);

#ifdef __cplusplus
}
#endif