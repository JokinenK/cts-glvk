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
	const CtsAllocationCallbacks* allocator;
	CtsDeviceSize blockSize;
    CtsDeviceSize growSize;
	CtsPoolAllocatorPool* pools;
    CtsPoolAllocatorBlock* blocks;
};

static CtsPoolAllocatorPool* allocatePool(CtsPoolAllocator pInstance)
{
	CtsPoolAllocatorPool* pool = ctsAllocation(
		pInstance->allocator,
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

static CtsPoolAllocatorBlock* splitPoolIntoBlocks(CtsPoolAllocator pInstance, CtsPoolAllocatorPool* pPool) {
    size_t availableSize = pInstance->growSize - sizeof(CtsPoolAllocatorPool);
    size_t blocksPerPool = availableSize / pInstance->blockSize;

    CtsPoolAllocatorBlock* firstBlock = (CtsPoolAllocatorBlock*)((char*)pPool + sizeof(CtsPoolAllocatorPool));
    CtsPoolAllocatorBlock* block = firstBlock;
    
    for (uint32_t i = 0; i < blocksPerPool; ++i) {
        block->next = (CtsPoolAllocatorBlock*)((char*)block + pInstance->blockSize);
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
	size_t pSize,
	size_t pAlign,
	CtsSystemAllocationScope pScope
) {
	(void) pScope;

	ctsPoolAllocatorAlloc((CtsPoolAllocator) pUserData, pSize, pAlign);
}

static void* proxyReallocation(
	void* pUserData,
	void* pOriginal,
	size_t pSize,
	size_t pAlign,
	CtsSystemAllocationScope pScope
) {
	(void) pScope;

	ctsPoolAllocatorRealloc((CtsPoolAllocator) pUserData, pOriginal, pSize, pAlign);
}

static void proxyFree(
	void* pUserData,
	void* pMemory
) {
	ctsPoolAllocatorFree((CtsPoolAllocator) pUserData, pMemory);
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

bool ctsCreatePoolAllocator(CtsPoolAllocator* pInstance, const CtsPoolAllocatorCreateInfo* pCreateInfo)
{	
	CtsPoolAllocator instance = ctsAllocation(
		pCreateInfo->allocator,
		sizeof(struct CtsPoolAllocator),
		alignof(struct CtsPoolAllocator),
		CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
	);

	if (instance == NULL) {
		return false;
	}

	instance->growSize = pCreateInfo->growSize;
	instance->allocator = pCreateInfo->allocator;
	instance->pools = allocatePool(instance);
    instance->blocks = splitPoolIntoBlocks(instance, instance->pools);

	if (instance->pools == NULL) {
		ctsFree(pCreateInfo->allocator, instance);
		return false;
	}
	
	return true;
}

void ctsGetPoolAllocatorCallbacks(CtsPoolAllocator pInstance, CtsAllocationCallbacks* pAllocator) {
	pAllocator->userData           = pInstance;
	pAllocator->allocation         = proxyAllocation;
	pAllocator->reallocation       = proxyReallocation;
	pAllocator->free               = proxyFree;
	pAllocator->internalAllocation = proxyInternalAllocation;
	pAllocator->internalFree       = proxyInternalFree;
}

bool ctsDestroyPoolAllocator(CtsPoolAllocator pInstance)
{
	CtsPoolAllocatorPool* pool = pInstance->pools;
	CtsPoolAllocatorPool* prev;

	while (pool != NULL) {
		prev = pool;
		pool = pool->next;

		ctsFree(pInstance->allocator, prev);
	}
	
	return true;
}

void* ctsPoolAllocatorAlloc(CtsPoolAllocator pInstance, size_t pSize, size_t pAlign)
{
    if (pSize > pInstance->blockSize) {
        return NULL;
    }

    if (pInstance->blocks == NULL) {
        CtsPoolAllocatorPool* prevPool = pInstance->pools;
        pInstance->pools = allocatePool(pInstance);
        pInstance->pools->next = prevPool;
        pInstance->blocks = splitPoolIntoBlocks(pInstance, pInstance->pools);
    }
    
    CtsPoolAllocatorBlock* freeBlock = pInstance->blocks;
    pInstance->blocks = freeBlock->next;

    return freeBlock;
}

void* ctsPoolAllocatorRealloc(CtsPoolAllocator pInstance, void* pOrigPtr, size_t pSize, size_t pAlign)
{
    if (pSize > pInstance->blockSize) {
        return NULL;
    }

    return pOrigPtr;
}

void ctsPoolAllocatorFree(CtsPoolAllocator pInstance, void* pPtr)
{
    CtsPoolAllocatorBlock* block = pPtr;
    block->next = pInstance->blocks;
    pInstance->blocks = block;
}

void ctsPoolAllocatorReset(CtsPoolAllocator pInstance)
{
	CtsPoolAllocatorPool* pool = pInstance->pools;
    pInstance->blocks = NULL;

	while (pool != NULL) {
        CtsPoolAllocatorBlock* first = splitPoolIntoBlocks(pInstance, pool);
        CtsPoolAllocatorBlock* last = getLastBlock(first);

        last->next = pInstance->blocks;
        pInstance->blocks = first;

		pool = pool->next;
	}
}

