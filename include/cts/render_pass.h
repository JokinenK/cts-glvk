#pragma once

#include "vulkan/vk_layer.h"
#include "vulkan/vulkan_core.h"
#include "cts/macros.h"

#ifdef __cplusplus
extern "C" {
#endif

VkResult VKAPI_CALL ctsCreateRenderPass(
    VkDevice device,
    const VkRenderPassCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkRenderPass* pRenderPass
);

void VKAPI_CALL ctsDestroyRenderPass(
    VkDevice device,
    VkRenderPass renderPass,
    const VkAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif