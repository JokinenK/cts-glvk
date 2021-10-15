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

CTS_DEFINE_TRAMPOLINE_VOID(
    DestroyPipeline,
    device,
    pipeline,
    pAllocator
);

#ifdef __cplusplus
}
#endif