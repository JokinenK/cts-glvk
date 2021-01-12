#pragma once

#include <stdbool.h>
#include <cts/allocator.h>
#include <cts/typedefs/device_size.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsLinearAllocator* CtsLinearAllocator;
typedef struct CtsLinearAllocatorCreateInfo {
    const CtsAllocationCallbacks* allocator;
	CtsDeviceSize growSize;
} CtsLinearAllocatorCreateInfo;

bool ctsCreateLinearAllocator(CtsLinearAllocator* pInstance, const CtsLinearAllocatorCreateInfo* pCreateInfo);
void ctsGetLinearAllocatorCallbacks(CtsLinearAllocator pInstance, CtsAllocationCallbacks* pAllocator);
bool ctsDestroyLinearAllocator(CtsLinearAllocator pInstance);

void* ctsLinearAllocatorAlloc(CtsLinearAllocator pInstance, size_t pSize, size_t pAlign);
void* ctsLinearAllocatorRealloc(CtsLinearAllocator pInstance, void* pOrigPtr, size_t pSize, size_t pAlign);
void ctsLinearAllocatorFree(CtsLinearAllocator pInstance, void* pPtr);
void ctsLinearAllocatorReset(CtsLinearAllocator pInstance);
void ctsLinearAllocatorTrim(CtsLinearAllocator pInstance);

#ifdef __cplusplus
}
#endif