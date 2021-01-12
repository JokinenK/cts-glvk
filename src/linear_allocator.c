#include <string.h>
#include <stddef.h>
#include <cts/align.h>
#include <cts/allocator.h>
#include <cts/linear_allocator.h>

typedef struct CtsLinearAllocatorPool {
	void* begin;
	void* end;
	void* current;
	void* prev;
	struct CtsLinearAllocatorPool* next;
} CtsLinearAllocatorPool;

struct CtsLinearAllocator {
	const CtsAllocationCallbacks* allocator;
	CtsDeviceSize growSize;
    CtsLinearAllocatorPool* begin;
	CtsLinearAllocatorPool* current;
};

static bool canFit(CtsLinearAllocatorPool* pPool, size_t pSize) {
	return ((char*)pPool->current + pSize < (char*)pPool->end);
}

static void* proxyAllocation(
	void* pUserData,
	size_t pSize,
	size_t pAlign,
	CtsSystemAllocationScope pScope
) {
	(void) pScope;

	ctsLinearAllocatorAlloc((CtsLinearAllocator) pUserData, pSize, pAlign);
}

static void* proxyReallocation(
	void* pUserData,
	void* pOriginal,
	size_t pSize,
	size_t pAlign,
	CtsSystemAllocationScope pScope
) {
	(void) pScope;

	ctsLinearAllocatorRealloc((CtsLinearAllocator) pUserData, pOriginal, pSize, pAlign);
}

static void proxyFree(
	void* pUserData,
	void* pMemory
) {
	ctsLinearAllocatorFree((CtsLinearAllocator) pUserData, pMemory);
}

static void proxyInternalAllocation(
	void* pUserData,
	size_t pSize,
	CtsSystemAllocationScope pScope
) {
	(void) pUserData;
	(void) pSize;
	(void) pScope;
}

static void proxyInternalFree(
	void* pUserData,
	size_t pSize, 
	CtsSystemAllocationScope pScope
) {
	(void) pUserData;
	(void) pSize;
	(void) pScope;
}

static CtsLinearAllocatorPool* allocatePool(CtsLinearAllocator pInstance)
{
	CtsLinearAllocatorPool* pool = ctsAllocation(
		pInstance->allocator,
		sizeof(CtsLinearAllocatorPool),
		alignof(CtsLinearAllocatorPool),
		CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
	);

	if (pool != NULL) {
		pool->begin = (char*)pool + sizeof(CtsLinearAllocatorPool);
		pool->end = (char*)pool + pInstance->growSize;
		pool->current = pool->begin;
		pool->prev = pool->begin;
		pool->next = NULL;

		return pool;
	}

	return NULL;
}

bool ctsCreateLinearAllocator(CtsLinearAllocator* pInstance, const CtsLinearAllocatorCreateInfo* pCreateInfo)
{	
	CtsLinearAllocator instance = ctsAllocation(
		pCreateInfo->allocator,
		sizeof(struct CtsLinearAllocator),
		alignof(struct CtsLinearAllocator),
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

void ctsGetLinearAllocatorCallbacks(CtsLinearAllocator pInstance, CtsAllocationCallbacks* pAllocator) {
	pAllocator->userData           = pInstance;
	pAllocator->allocation         = proxyAllocation;
	pAllocator->reallocation       = proxyReallocation;
	pAllocator->free               = proxyFree;
	pAllocator->internalAllocation = proxyInternalAllocation;
	pAllocator->internalFree       = proxyInternalFree;
}

bool ctsDestroyLinearAllocator(CtsLinearAllocator pInstance)
{
	CtsLinearAllocatorPool* pool = pInstance->begin;
	CtsLinearAllocatorPool* prev;

	while (pool != NULL) {
		prev = pool;
		pool = pool->next;

		ctsFree(pInstance->allocator, prev);
	}
	
	return true;
}

void* ctsLinearAllocatorAlloc(CtsLinearAllocator pInstance, size_t pSize, size_t pAlign)
{
	size_t alignedSize = pSize;
	CtsLinearAllocatorPool* pool = pInstance->current;

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

void* ctsLinearAllocatorRealloc(CtsLinearAllocator pInstance, void* pOrigPtr, size_t pSize, size_t pAlign)
{
	if (pOrigPtr == NULL) {
		return ctsLinearAllocatorAlloc(pInstance, pSize, pAlign);
	}
	
	size_t alignedSize = pSize;
	if (pInstance->current->prev == pOrigPtr) {
		pInstance->current->current = (char*)pInstance->current->prev + alignedSize;
		return pOrigPtr;
	}

	return ctsLinearAllocatorAlloc(pInstance, pSize, pAlign);
}

void ctsLinearAllocatorFree(CtsLinearAllocator pInstance, void* pPtr)
{
}

void ctsLinearAllocatorReset(CtsLinearAllocator pInstance)
{
	CtsLinearAllocatorPool* pool = pInstance->begin;

	while (pool != NULL) {
		pool->current = pool->begin;
		pool->prev = pool->begin;
		pool = pool->next;
	}

	pInstance->current = pInstance->begin;
}

void ctsLinearAllocatorTrim(CtsLinearAllocator pInstance)
{
	CtsLinearAllocatorPool* pool = pInstance->current->next;
	CtsLinearAllocatorPool* prev;

	while (pool != NULL) {
		prev = pool;
		pool = pool->next;

		ctsFree(pInstance->allocator, prev);
	}

	pInstance->current->next = NULL;
}
