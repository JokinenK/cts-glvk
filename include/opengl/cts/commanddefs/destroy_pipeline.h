#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDestroyPipeline {
    CtsCmdBase base;
    VkDevice device;
    VkPipeline pipeline;
    const VkAllocationCallbacks* pAllocator;
} CtsDestroyPipeline;

#ifdef __cplusplus
}
#endif