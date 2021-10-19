#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/pipeline_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDestroyPipeline {
    CtsCmdBase base;
    VkDevice device;
    VkPipeline pipeline;
    const VkAllocationCallbacks* pAllocator;
} CtsDestroyPipeline;

CTS_DEFINE_COMMAND_TRAMPOLINE_VOID(
    CtsDestroyPipeline,
    ctsDestroyPipelineImpl,
    device,
    pipeline,
    pAllocator
);

CTS_DEFINE_COMMAND_METADATA(CtsDestroyPipeline);

#ifdef __cplusplus
}
#endif