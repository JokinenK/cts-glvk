#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDestroyPipelineLayout {
    CtsCmdBase base;
    VkDevice device;
    VkPipelineLayout pipelineLayout;
    const VkAllocationCallbacks* pAllocator;
} CtsDestroyPipelineLayout;

#ifdef __cplusplus
}
#endif