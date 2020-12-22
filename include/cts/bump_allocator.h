#pragma once

#include <stdbool.h>
#include <cts/allocator.h>
#include <cts/typedefs/device_size.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsBumpAllocator* CtsBumpAllocator;
typedef struct CtsBumpAllocatorCreateInfo {
    const CtsAllocationCallbacks* allocator;
	CtsDeviceSize growSize;
} CtsBumpAllocatorCreateInfo;

bool ctsCreateBumpAllocator(CtsBumpAllocator* pInstance, const CtsBumpAllocatorCreateInfo* pCreateInfo);
bool ctsDestroyBumpAllocator(CtsBumpAllocator pInstance);

void* ctsBumpAllocatorAlloc(CtsBumpAllocator pInstance, size_t pSize, size_t pAlign);
void* ctsBumpAllocatorRealloc(CtsBumpAllocator pInstance, void* pOrigPtr, size_t pSize, size_t pAlign);
void ctsBumpAllocatorFree(CtsBumpAllocator pInstance, void* pPtr);

void ctsBumpAllocatorReset(CtsBumpAllocator pInstance);
void ctsBumpAllocatorTrim(CtsBumpAllocator pInstance);

#ifdef __cplusplus
}
#endif