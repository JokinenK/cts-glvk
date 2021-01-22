#pragma once

#include <stdbool.h>
#include <cts/allocator.h>
#include <cts/typedefs/device_size.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsPoolAllocatorImpl* CtsPoolAllocator;
typedef struct CtsPoolAllocatorCreateInfo {
    const CtsAllocationCallbacks* pAllocator;
	CtsDeviceSize blockSize;
    CtsDeviceSize growSize;
} CtsPoolAllocatorCreateInfo;

bool ctsCreatePoolAllocator(CtsPoolAllocator* pInstance, const CtsPoolAllocatorCreateInfo* pCreateInfo);
void ctsGetPoolAllocatorCallbacks(CtsPoolAllocator instance, CtsAllocationCallbacks* pAllocator);
bool ctsDestroyPoolAllocator(CtsPoolAllocator instance);

void* ctsPoolAllocatorAlloc(CtsPoolAllocator instance, size_t size, size_t align);
void* ctsPoolAllocatorRealloc(CtsPoolAllocator instance, void* pOrigPtr, size_t size, size_t align);
void ctsPoolAllocatorFree(CtsPoolAllocator instance, void* pPtr);
void ctsPoolAllocatorReset(CtsPoolAllocator instance);

#ifdef __cplusplus
}
#endif