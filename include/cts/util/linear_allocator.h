#pragma once

#include <stdbool.h>
#include "cts/allocator.h"
#include "vulkan/vulkan_core.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsLinearAllocatorImpl* CtsLinearAllocator;
typedef struct CtsLinearAllocatorCreateInfo {
    const VkAllocationCallbacks* pAllocator;
	VkDeviceSize growSize;
} CtsLinearAllocatorCreateInfo;

bool ctsCreateLinearAllocator(CtsLinearAllocator* pInstance, const CtsLinearAllocatorCreateInfo* pCreateInfo);
void ctsGetLinearAllocatorCallbacks(CtsLinearAllocator instance, VkAllocationCallbacks* pAllocator);
bool ctsDestroyLinearAllocator(CtsLinearAllocator instance);

void* ctsLinearAllocatorAlloc(CtsLinearAllocator instance, size_t size, size_t align);
void* ctsLinearAllocatorRealloc(CtsLinearAllocator instance, void* pOrigPtr, size_t size, size_t align);
void ctsLinearAllocatorFree(CtsLinearAllocator instance, void* pPtr);
void ctsLinearAllocatorReset(CtsLinearAllocator instance);
void ctsLinearAllocatorTrim(CtsLinearAllocator instance);

#ifdef __cplusplus
}
#endif