#pragma once

#include <stdbool.h>
#include "glad/glad.h"
#include "vulkan/vulkan_core.h"
#include "cts/gl_pipeline.h"
#include "private/object_base.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsPipeline {
    struct CtsObjectBase base;
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