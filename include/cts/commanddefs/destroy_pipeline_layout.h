#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/pipeline_layout_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDestroyPipelineLayout {
    CtsCmdBase base;
    VkDevice device;
    VkPipelineLayout pipelineLayout;
    const VkAllocationCallbacks* pAllocator;
} CtsDestroyPipelineLayout;

CTS_DEFINE_TRAMPOLINE_VOID(
    DestroyPipelineLayout,
    device,
    pipelineLayout,
    pAllocator
);

#ifdef __cplusplus
}
#endif