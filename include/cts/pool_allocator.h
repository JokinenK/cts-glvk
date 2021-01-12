#pragma once

#include <stdbool.h>
#include <cts/allocator.h>
#include <cts/typedefs/device_size.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsPoolAllocator* CtsPoolAllocator;
typedef struct CtsPoolAllocatorCreateInfo {
    const CtsAllocationCallbacks* allocator;
	CtsDeviceSize blockSize;
    CtsDeviceSize growSize;
} CtsPoolAllocatorCreateInfo;

bool ctsCreatePoolAllocator(CtsPoolAllocator* pInstance, const CtsPoolAllocatorCreateInfo* pCreateInfo);
void ctsGetPoolAllocatorCallbacks(CtsPoolAllocator pInstance, CtsAllocationCallbacks* pAllocator);
bool ctsDestroyPoolAllocator(CtsPoolAllocator pInstance);

void* ctsPoolAllocatorAlloc(CtsPoolAllocator pInstance, size_t pSize, size_t pAlign);
void* ctsPoolAllocatorRealloc(CtsPoolAllocator pInstance, void* pOrigPtr, size_t pSize, size_t pAlign);
void ctsPoolAllocatorFree(CtsPoolAllocator pInstance, void* pPtr);
void ctsPoolAllocatorReset(CtsPoolAllocator pInstance);

#ifdef __cplusplus
}
#endif