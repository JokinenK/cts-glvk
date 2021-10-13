#pragma once

#include <stdint.h>
#include "vulkan/vk_layer.h"
#include "vulkan/vulkan_core.h"
#include "cts/macros.h"

#ifdef __cplusplus
extern "C" {
#endif

VkResult VKAPI_CALL ctsCreateGraphicsPipelines(
    VkDevice device,
    VkPipelineCache pipelineCache,
    uint32_t createInfoCount,
    const VkGraphicsPipelineCreateInfo* pCreateInfos,
    const VkAllocationCallbacks* pAllocator,
    VkPipeline* pPipelines
);

void VKAPI_CALL ctsDestroyPipeline(
    VkDevice device,
    VkPipeline pipeline,
    const VkAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif