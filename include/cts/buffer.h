#pragma once

#include "vulkan/vulkan_core.h"
#include "vulkan/vk_layer.h"
#include "cts/macros.h"

#ifdef __cplusplus
extern "C" {
#endif

LIBRARY_API VkResult VKAPI_CALL ctsCreateBuffer(
    VkDevice device,
    const VkBufferCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkBuffer* pBuffer
);

LIBRARY_API void VKAPI_CALL ctsDestroyBuffer(
    VkDevice device,
    VkBuffer buffer,
    const VkAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif