#include <string.h>
#include <stddef.h>
#include <cts/align.h>
#include <cts/allocator.h>
#include <cts/Pool_allocator.h>

typedef struct CtsPoolAllocatorBlock {
	struct CtsPoolAllocatorBlock* next;
} CtsPoolAllocatorBlock;

typedef struct CtsPoolAllocatorPool {
	struct CtsPoolAllocatorPool* next;
} CtsPoolAllocatorPool;

struct CtsPoolAllocator {
	const CtsAllocationCallbacks* pAllocator;
	CtsDeviceSize blockSize;
    CtsDeviceSize growSize;
	CtsPoolAllocatorPool* pools;
    CtsPoolAllocatorBlock* blocks;
};

static CtsPoolAllocatorPool* allocatePool(CtsPoolAllocator instance)
{
	CtsPoolAllocatorPool* pool = ctsAllocation(
		instance->pAllocator,
		sizeof(CtsPoolAllocatorPool),
		alignof(CtsPoolAllocatorPool),
		CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
	);

	if (pool != NULL) {
        pool->next = NULL;
		return pool;
	}

	return NULL;
}

static CtsPoolAllocatorBlock* splitPoolIntoBlocks(CtsPoolAllocator instance, CtsPoolAllocatorPool* pPool) {
    size_t availableSize = instance->growSize - sizeof(CtsPoolAllocatorPool);
    size_t blocksPerPool = availableSize / instance->blockSize;

    CtsPoolAllocatorBlock* firstBlock = (CtsPoolAllocatorBlock*)((char*)pPool + sizeof(CtsPoolAllocatorPool));
    CtsPoolAllocatorBlock* block = firstBlock;
    
    for (uint32_t i = 0; i < blocksPerPool; ++i) {
        block->next = (CtsPoolAllocatorBlock*)((char*)block + instance->blockSize);
        block = block->next;
    }
    
    block->next = NULL;
    return firstBlock;
}

static CtsPoolAllocatorBlock* getLastBlock(CtsPoolAllocatorBlock* pBlock) {
    CtsPoolAllocatorBlock* block = pBlock;
    for (; block->next != NULL; block = block->next);
    return block;
}


static void* proxyAllocation(
	void* pUserData,
	size_t size,
	size_t align,
	CtsSystemAllocationScope scope
) {
	(void) scope;

	return ctsPoolAllocatorAlloc((CtsPoolAllocator) pUserData, size, align);
}

static void* proxyReallocation(
	void* pUserData,
	void* pOriginal,
	size_t size,
	size_t align,
	CtsSystemAllocationScope scope
) {
	(void) scope;

	return ctsPoolAllocatorRealloc((CtsPoolAllocator) pUserData, pOriginal, size, align);
}

static void proxyFree(
	void* pUserData,
	void* pMemory
) {
	ctsPoolAllocatorFree((CtsPoolAllocator) pUserData, pMemory);
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

bool ctsCreatePoolAllocator(CtsPoolAllocator* pInstance, const CtsPoolAllocatorCreateInfo* pCreateInfo)
{	
	CtsPoolAllocator instance = ctsAllocation(
		pCreateInfo->pAllocator,
		sizeof(struct CtsPoolAllocator),
		alignof(struct CtsPoolAllocator),
		CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
	);

	if (instance == NULL) {
		return false;
	}

	instance->growSize = pCreateInfo->growSize;
	instance->pAllocator = pCreateInfo->pAllocator;
	instance->pools = allocatePool(instance);
    instance->blocks = splitPoolIntoBlocks(instance, instance->pools);

	if (instance->pools == NULL) {
		ctsFree(pCreateInfo->pAllocator, instance);
		return false;
	}
	
	return true;
}

void ctsGetPoolAllocatorCallbacks(CtsPoolAllocator instance, CtsAllocationCallbacks* pAllocator) {
	pAllocator->userData           = instance;
	pAllocator->allocation         = proxyAllocation;
	pAllocator->reallocation       = proxyReallocation;
	pAllocator->free               = proxyFree;
	pAllocator->internalAllocation = proxyInternalAllocation;
	pAllocator->internalFree       = proxyInternalFree;
}

bool ctsDestroyPoolAllocator(CtsPoolAllocator instance)
{
	CtsPoolAllocatorPool* pool = instance->pools;
	CtsPoolAllocatorPool* prev;

	while (pool != NULL) {
		prev = pool;
		pool = pool->next;

		ctsFree(instance->pAllocator, prev);
	}
	
	return true;
}

void* ctsPoolAllocatorAlloc(CtsPoolAllocator instance, size_t size, size_t align)
{
    if (size > instance->blockSize) {
        return NULL;
    }

    if (instance->blocks == NULL) {
        CtsPoolAllocatorPool* prevPool = instance->pools;
        instance->pools = allocatePool(instance);
        instance->pools->next = prevPool;
        instance->blocks = splitPoolIntoBlocks(instance, instance->pools);
    }
    
    CtsPoolAllocatorBlock* freeBlock = instance->blocks;
    instance->blocks = freeBlock->next;

    return freeBlock;
}

void* ctsPoolAllocatorRealloc(CtsPoolAllocator instance, void* pOrigPtr, size_t size, size_t align)
{
    if (size > instance->blockSize) {
        return NULL;
    }

    return pOrigPtr;
}

void ctsPoolAllocatorFree(CtsPoolAllocator instance, void* pPtr)
{
    CtsPoolAllocatorBlock* block = pPtr;
    block->next = instance->blocks;
    instance->blocks = block;
}

void ctsPoolAllocatorReset(CtsPoolAllocator instance)
{
	CtsPoolAllocatorPool* pool = instance->pools;
    instance->blocks = NULL;

	while (pool != NULL) {
        CtsPoolAllocatorBlock* first = splitPoolIntoBlocks(instance, pool);
        CtsPoolAllocatorBlock* last = getLastBlock(first);

        last->next = instance->blocks;
        instance->blocks = first;

		pool = pool->next;
	}
}

