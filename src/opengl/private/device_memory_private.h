#pragma once

#include "glad/glad.h"
#include "vulkan/vulkan_core.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsDeviceMemory {
    GLenum handle;
    VkDeviceSize size;
    uint32_t heapIndex;
    VkMemoryPropertyFlags propertyFlags;
    const VkAllocationCallbacks* pAllocator;

    struct CtsImage* images;
    void* pMappedMemory;
};

VkResult ctsAllocateMemoryImpl(
    VkDevice device,
    const VkMemoryAllocateInfo* pAllocateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDeviceMemory* pMemory
);

VkResult ctsMapMemoryImpl(
    VkDevice device,
    VkDeviceMemory memory,
    VkDeviceSize offset,
    VkDeviceSize size,
    VkMemoryMapFlags flags,
    void** ppData
);

void ctsUnmapMemoryImpl(
    VkDevice device,
    VkDeviceMemory memory
);

VkResult ctsFlushMappedMemoryRangesImpl(
    VkDevice device,
    uint32_t memoryRangeCount,
    const VkMappedMemoryRange* pMemoryRanges
);

void ctsFreeMemoryImpl(
    VkDevice device,
    VkDeviceMemory memory,
    const VkAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif