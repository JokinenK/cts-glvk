#pragma once

#include <stdbool.h>
#include <cts/allocator.h>
#include <cts/typedefs/device_size.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsLinearAllocator* CtsLinearAllocator;
typedef struct CtsLinearAllocatorCreateInfo {
    const CtsAllocationCallbacks* pAllocator;
	CtsDeviceSize growSize;
} CtsLinearAllocatorCreateInfo;

bool ctsCreateLinearAllocator(CtsLinearAllocator* pInstance, const CtsLinearAllocatorCreateInfo* pCreateInfo);
void ctsGetLinearAllocatorCallbacks(CtsLinearAllocator instance, CtsAllocationCallbacks* pAllocator);
bool ctsDestroyLinearAllocator(CtsLinearAllocator instance);

void* ctsLinearAllocatorAlloc(CtsLinearAllocator instance, size_t size, size_t align);
void* ctsLinearAllocatorRealloc(CtsLinearAllocator instance, void* pOrigPtr, size_t size, size_t align);
void ctsLinearAllocatorFree(CtsLinearAllocator instance, void* pPtr);
void ctsLinearAllocatorReset(CtsLinearAllocator instance);
void ctsLinearAllocatorTrim(CtsLinearAllocator instance);

#ifdef __cplusplus
}
#endif