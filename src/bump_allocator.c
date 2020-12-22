#include <string.h>
#include <stddef.h>
#include <cts/align.h>
#include <cts/allocator.h>
#include <cts/bump_allocator.h>

typedef struct CtsBumpAllocatorPool {
	void* begin;
	void* end;
	void* current;
	void* prev;
	struct CtsBumpAllocatorPool* next;
} CtsBumpAllocatorPool;

struct CtsBumpAllocator {
	const CtsAllocationCallbacks* allocator;
	CtsDeviceSize growSize;
    CtsBumpAllocatorPool* begin;
	CtsBumpAllocatorPool* current;
};

static bool canFit(CtsBumpAllocatorPool* pPool, size_t pSize) {
	return ((char*)pPool->current + pSize < (char*)pPool->end);
}

static CtsBumpAllocatorPool* allocatePool(CtsBumpAllocator pInstance)
{
	CtsBumpAllocatorPool* pool = ctsAllocation(
		pInstance->allocator,
		sizeof(CtsBumpAllocatorPool),
		alignof(CtsBumpAllocatorPool),
		CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
	);

	if (pool != NULL) {
		pool->begin = (char*)pool + sizeof(CtsBumpAllocatorPool);
		pool->end = (char*)pool + pInstance->growSize;
		pool->current = pool->begin;
		pool->prev = pool->begin;
		pool->next = NULL;

		return pool;
	}

	return NULL;
}

bool ctsCreateBumpAllocator(CtsBumpAllocator* pInstance, const CtsBumpAllocatorCreateInfo* pCreateInfo)
{	
	CtsBumpAllocator instance = ctsAllocation(
		pCreateInfo->allocator,
		sizeof(struct CtsBumpAllocator),
		alignof(struct CtsBumpAllocator),
		CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
	);

	if (instance == NULL) {
		return false;
	}

	instance->growSize = pCreateInfo->growSize;
	instance->allocator = pCreateInfo->allocator;
	instance->begin = allocatePool(instance);
	instance->current = instance->begin;

	if (instance->begin == NULL) {
		ctsFree(pCreateInfo->allocator, instance);
		return false;
	}
	
	return true;
}

bool ctsDestroyBumpAllocator(CtsBumpAllocator pInstance)
{
	CtsBumpAllocatorPool* pool = pInstance->begin;
	CtsBumpAllocatorPool* prev;

	while (pool != NULL) {
		prev = pool;
		pool = pool->next;

		ctsFree(pInstance->allocator, prev);
	}
	
	return true;
}

void* ctsBumpAllocatorAlloc(CtsBumpAllocator pInstance, size_t pSize, size_t pAlign)
{
	size_t alignedSize = pSize;
	CtsBumpAllocatorPool* pool = pInstance->current;

	while (pool != NULL) {	
		if (canFit(pool, alignedSize)) {
			pool->prev = pool->current;
			pool->current = (char*)pool->current + alignedSize;
			
			return pool->prev;
		}

		if (pool->next == NULL) {
			pool->next = allocatePool(pInstance);
		}

		pool = pool->next;
	}

    return NULL;
}

void* ctsBumpAllocatorRealloc(CtsBumpAllocator pInstance, void* pOrigPtr, size_t pSize, size_t pAlign)
{
	if (pOrigPtr == NULL) {
		return ctsBumpAllocatorAlloc(pInstance, pSize, pAlign);
	}
	
	size_t alignedSize = pSize;
	if (pInstance->current->prev == pOrigPtr) {
		pInstance->current->current = (char*)pInstance->current->prev + alignedSize;
		return pOrigPtr;
	}

	return ctsBumpAllocatorAlloc(pInstance, pSize, pAlign);
}

void ctsBumpAllocatorFree(CtsBumpAllocator pInstance, void* pPtr)
{
}

void ctsBumpAllocatorReset(CtsBumpAllocator pInstance)
{
	CtsBumpAllocatorPool* pool = pInstance->begin;

	while (pool != NULL) {
		pool->current = pool->begin;
		pool->prev = pool->begin;
		pool = pool->next;
	}

	pInstance->current = pInstance->begin;
}

void ctsBumpAllocatorTrim(CtsBumpAllocator pInstance)
{
	CtsBumpAllocatorPool* pool = pInstance->current->next;
	CtsBumpAllocatorPool* prev;

	while (pool != NULL) {
		prev = pool;
		pool = pool->next;

		ctsFree(pInstance->allocator, prev);
	}

	pInstance->current->next = NULL;
}
