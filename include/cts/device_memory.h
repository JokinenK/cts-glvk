#pragma once

#include "vulkan/vulkan_core.h"

#ifdef __cplusplus
extern "C" {
#endif

VkResult ctsAllocateMemory(
    VkDevice device,
    const VkMemoryAllocateInfo* pAllocateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDeviceMemory* pMemory
);

VkResult ctsMapMemory(
    VkDevice device,
    VkDeviceMemory memory,
    VkDeviceSize offset,
    VkDeviceSize size,
    VkMemoryMapFlags flags,
    void** pData
);

void ctsUnmapMemory(
    VkDevice device,
    VkDeviceMemory memory
);

VkResult ctsFlushMappedMemoryRanges(
    VkDevice device,
    uint32_t memoryRangeCount,
    const VkMappedMemoryRange* pMemoryRanges
);

void ctsFreeMemory(
    VkDevice device,
    VkDeviceMemory memory,
    const VkAllocationCallbacks* pAllocator
);

VkResult ctsBindBufferMemory(
    VkDevice device,
    VkBuffer buffer, 
    VkDeviceMemory memory,
    VkDeviceSize offset
);

VkResult ctsBindImageMemory(
    VkDevice device,
    VkImage image, 
    VkDeviceMemory memory,
    VkDeviceSize offset
);

void ctsGetBufferMemoryRequirements(
    VkDevice device,
    VkBuffer buffer,
    VkMemoryRequirements* pMemoryRequirements
);

void ctsGetImageMemoryRequirements(
    VkDevice device,
    VkImage image,
    VkMemoryRequirements* pMemoryRequirements
);

#ifdef __cplusplus
}
#endif