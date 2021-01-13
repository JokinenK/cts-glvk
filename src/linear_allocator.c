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
	const CtsAllocationCallbacks* pAllocator;
	CtsDeviceSize growSize;
    CtsLinearAllocatorPool* begin;
	CtsLinearAllocatorPool* current;
};

static bool canFit(CtsLinearAllocatorPool* pPool, size_t size) {
	return ((char*)pPool->current + size < (char*)pPool->end);
}

static void* proxyAllocation(
	void* pUserData,
	size_t size,
	size_t align,
	CtsSystemAllocationScope scope
) {
	(void) scope;

	return ctsLinearAllocatorAlloc((CtsLinearAllocator) pUserData, size, align);
}

static void* proxyReallocation(
	void* pUserData,
	void* pOriginal,
	size_t size,
	size_t align,
	CtsSystemAllocationScope scope
) {
	(void) scope;

	return ctsLinearAllocatorRealloc((CtsLinearAllocator) pUserData, pOriginal, size, align);
}

static void proxyFree(
	void* pUserData,
	void* pMemory
) {
	ctsLinearAllocatorFree((CtsLinearAllocator) pUserData, pMemory);
}

static void proxyInternalAllocation(
	void* pUserData,
	size_t size,
	CtsSystemAllocationScope scope
) {
	(void) pUserData;
	(void) size;
	(void) scope;
}

static void proxyInternalFree(
	void* pUserData,
	size_t size, 
	CtsSystemAllocationScope scope
) {
	(void) pUserData;
	(void) size;
	(void) scope;
}

static CtsLinearAllocatorPool* allocatePool(CtsLinearAllocator instance)
{
	CtsLinearAllocatorPool* pool = ctsAllocation(
		instance->pAllocator,
		sizeof(CtsLinearAllocatorPool),
		alignof(CtsLinearAllocatorPool),
		CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
	);

	if (pool != NULL) {
		pool->begin = (char*)pool + sizeof(CtsLinearAllocatorPool);
		pool->end = (char*)pool + instance->growSize;
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
		pCreateInfo->pAllocator,
		sizeof(struct CtsLinearAllocator),
		alignof(struct CtsLinearAllocator),
		CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
	);

	if (instance == NULL) {
		return false;
	}

	instance->growSize = pCreateInfo->growSize;
	instance->pAllocator = pCreateInfo->pAllocator;
	instance->begin = allocatePool(instance);
	instance->current = instance->begin;

	if (instance->begin == NULL) {
		ctsFree(pCreateInfo->pAllocator, instance);
		return false;
	}
	
	return true;
}

void ctsGetLinearAllocatorCallbacks(CtsLinearAllocator instance, CtsAllocationCallbacks* pAllocator) {
	pAllocator->userData           = instance;
	pAllocator->allocation         = proxyAllocation;
	pAllocator->reallocation       = proxyReallocation;
	pAllocator->free               = proxyFree;
	pAllocator->internalAllocation = proxyInternalAllocation;
	pAllocator->internalFree       = proxyInternalFree;
}

bool ctsDestroyLinearAllocator(CtsLinearAllocator instance)
{
	CtsLinearAllocatorPool* pool = instance->begin;
	CtsLinearAllocatorPool* prev;

	while (pool != NULL) {
		prev = pool;
		pool = pool->next;

		ctsFree(instance->pAllocator, prev);
	}
	
	return true;
}

void* ctsLinearAllocatorAlloc(CtsLinearAllocator instance, size_t size, size_t align)
{
	size_t alignedSize = size;
	CtsLinearAllocatorPool* pool = instance->current;

	while (pool != NULL) {	
		if (canFit(pool, alignedSize)) {
			pool->prev = pool->current;
			pool->current = (char*)pool->current + alignedSize;
			
			return pool->prev;
		}

		if (pool->next == NULL) {
			pool->next = allocatePool(instance);
		}

		pool = pool->next;
	}

    return NULL;
}

void* ctsLinearAllocatorRealloc(CtsLinearAllocator instance, void* pOrigPtr, size_t size, size_t align)
{
	if (pOrigPtr == NULL) {
		return ctsLinearAllocatorAlloc(instance, size, align);
	}
	
	size_t alignedSize = size;
	if (instance->current->prev == pOrigPtr) {
		instance->current->current = (char*)instance->current->prev + alignedSize;
		return pOrigPtr;
	}

	return ctsLinearAllocatorAlloc(instance, size, align);
}

void ctsLinearAllocatorFree(CtsLinearAllocator instance, void* pPtr)
{
}

void ctsLinearAllocatorReset(CtsLinearAllocator instance)
{
	CtsLinearAllocatorPool* pool = instance->begin;

	while (pool != NULL) {
		pool->current = pool->begin;
		pool->prev = pool->begin;
		pool = pool->next;
	}

	instance->current = instance->begin;
}

void ctsLinearAllocatorTrim(CtsLinearAllocator instance)
{
	CtsLinearAllocatorPool* pool = instance->current->next;
	CtsLinearAllocatorPool* prev;

	while (pool != NULL) {
		prev = pool;
		pool = pool->next;

		ctsFree(instance->pAllocator, prev);
	}

	instance->current->next = NULL;
}
