#pragma once

#include <stdbool.h>
#include "cts/allocator.h"
#include "vulkan/vulkan_core.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsPoolAllocatorImpl* CtsPoolAllocator;
typedef struct CtsPoolAllocatorCreateInfo {
    const VkAllocationCallbacks* pAllocator;
	VkDeviceSize blockSize;
    VkDeviceSize growSize;
} CtsPoolAllocatorCreateInfo;

bool ctsCreatePoolAllocator(CtsPoolAllocator* pInstance, const CtsPoolAllocatorCreateInfo* pCreateInfo);
void ctsGetPoolAllocatorCallbacks(CtsPoolAllocator instance, VkAllocationCallbacks* pAllocator);
bool ctsDestroyPoolAllocator(CtsPoolAllocator instance);

void* ctsPoolAllocatorAlloc(CtsPoolAllocator instance, size_t size, size_t align);
void* ctsPoolAllocatorRealloc(CtsPoolAllocator instance, void* pOrigPtr, size_t size, size_t align);
void ctsPoolAllocatorFree(CtsPoolAllocator instance, void* pPtr);
void ctsPoolAllocatorReset(CtsPoolAllocator instance);

#ifdef __cplusplus
}
#endif