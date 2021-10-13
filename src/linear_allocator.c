#include <string.h>
#include <stddef.h>
#include "cts/allocator.h"
#include "cts/util/align.h"
#include "cts/util/linear_allocator.h"

typedef struct CtsLinearAllocatorPool {
	void* begin;
	void* end;
	void* current;
	struct CtsLinearAllocatorPool* next;
} CtsLinearAllocatorPool;

struct CtsLinearAllocatorImpl {
	const VkAllocationCallbacks* pAllocator;
	VkDeviceSize growSize;
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
	VkSystemAllocationScope scope
) {
	(void) scope;

	return ctsLinearAllocatorAlloc((CtsLinearAllocator) pUserData, size, align);
}

static void* proxyReallocation(
	void* pUserData,
	void* pOriginal,
	size_t size,
	size_t align,
	VkSystemAllocationScope scope
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
	VkInternalAllocationType allocationType,
	VkSystemAllocationScope allocationScope
) {
	(void) pUserData;
	(void) size;
	(void) allocationType;
	(void) allocationScope;
}

static void proxyInternalFree(
	void* pUserData,
	size_t size, 
	VkInternalAllocationType allocationType,
	VkSystemAllocationScope allocationScope
) {
	(void) pUserData;
	(void) size;
	(void) allocationType;
	(void) allocationScope;
}

static CtsLinearAllocatorPool* allocatePool(CtsLinearAllocator instance)
{
	CtsLinearAllocatorPool* pool = ctsAllocation(
		instance->pAllocator,
		instance->growSize,
		alignof(CtsLinearAllocatorPool),
		VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
	);

	if (pool != NULL) {
		pool->begin = (char*)pool + sizeof(CtsLinearAllocatorPool);
		pool->end = (char*)pool + instance->growSize;
		pool->current = pool->begin;
		pool->next = NULL;

		return pool;
	}

	return NULL;
}

bool ctsCreateLinearAllocator(CtsLinearAllocator* pInstance, const CtsLinearAllocatorCreateInfo* pCreateInfo)
{	
	CtsLinearAllocator instance = ctsAllocation(
		pCreateInfo->pAllocator,
		sizeof(struct CtsLinearAllocatorImpl),
		alignof(struct CtsLinearAllocatorImpl),
		VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
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

    *pInstance = instance;	
	return true;
}

void ctsGetLinearAllocatorCallbacks(CtsLinearAllocator instance, VkAllocationCallbacks* pAllocator) {
	pAllocator->pUserData             = instance;
	pAllocator->pfnAllocation         = proxyAllocation;
	pAllocator->pfnReallocation       = proxyReallocation;
	pAllocator->pfnFree               = proxyFree;
	pAllocator->pfnInternalAllocation = proxyInternalAllocation;
	pAllocator->pfnInternalFree       = proxyInternalFree;
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
			void* pPtr = pool->current;
			pool->current = (char*)pool->current + alignedSize;

			return pPtr;
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
	return NULL;
}

void ctsLinearAllocatorFree(CtsLinearAllocator instance, void* pPtr)
{
}

void ctsLinearAllocatorReset(CtsLinearAllocator instance)
{
	CtsLinearAllocatorPool* pool = instance->begin;

	while (pool != NULL) {
		pool->current = pool->begin;
		pool = pool->next;
	}

	instance->current = instance->begin;
}

void ctsLinearAllocatorTrim(CtsLinearAllocator instance)
{
	CtsLinearAllocatorPool* pool = instance->current;
	CtsLinearAllocatorPool* prev;

	if (pool->current != pool->begin) {
		pool = pool->next;
	}

	while (pool != NULL) {
		prev = pool;
		pool = pool->next;

		ctsFree(instance->pAllocator, prev);
	}

	instance->current->next = NULL;
}
