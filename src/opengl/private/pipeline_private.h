#pragma once

#include <stdbool.h>
#include "glad/glad.h"
#include "vulkan/vulkan_core.h"
#include "cts/typedefs/gl_pipeline.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsPipeline {
    VkPipelineBindPoint bindPoint;

    union {
        CtsGlGraphicsPipeline* graphics;
    };
};

VkResult ctsCreateGraphicsPipelinesImpl(
    VkDevice device,
    VkPipelineCache pipelineCache,
    uint32_t createInfoCount,
    const VkGraphicsPipelineCreateInfo* pCreateInfos,
    const VkAllocationCallbacks* pAllocator,
    VkPipeline* pPipelines
);

void ctsDestroyPipelineImpl(
    VkDevice device,
    VkPipeline pipeline,
    const VkAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif