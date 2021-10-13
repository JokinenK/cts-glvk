#pragma once

#include <stdint.h>
#include "vulkan/vk_layer.h"
#include "vulkan/vulkan_core.h"
#include "cts/macros.h"

#ifdef __cplusplus
extern "C" {
#endif

VkResult VKAPI_CALL ctsCreateCommandPool(
    VkDevice device,
    const VkCommandPoolCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkCommandPool* pCommandPool
);

void VKAPI_CALL ctsDestroyCommandPool(
    VkDevice device,
    VkCommandPool commandPool,
    const VkAllocationCallbacks* pAllocator
);

VkResult VKAPI_CALL ctsAllocateCommandBuffers(
    VkDevice device,
    const VkCommandBufferAllocateInfo* pAllocateInfo,
    VkCommandBuffer* pCommandBuffers
);

void VKAPI_CALL ctsFreeCommandBuffers(
    VkDevice device,
    VkCommandPool commandPool,
    uint32_t commandBufferCount,
    const VkCommandBuffer* pCommandBuffers
);

#ifdef __cplusplus
}
#endif