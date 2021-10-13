#pragma once

#include "vulkan/vk_layer.h"
#include "vulkan/vulkan_core.h"
#include "cts/macros.h"

#ifdef __cplusplus
extern "C" {
#endif

VkResult VKAPI_CALL ctsCreateShaderModule(
    VkDevice device,
    const VkShaderModuleCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkShaderModule* pShaderModule
);

void VKAPI_CALL ctsDestroyShaderModule(
    VkDevice device,
    VkShaderModule shaderModule,
    const VkAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif