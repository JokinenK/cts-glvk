#include <cts/bump_allocator_proxy.h>

static void* proxyAllocation(
	void* pUserData,
	size_t pSize,
	size_t pAlign,
	CtsSystemAllocationScope pScope
);

static void* proxyReallocation(
	void* pUserData,
	void* pOriginal,
	size_t pSize,
	size_t pAlign,
	CtsSystemAllocationScope pScope
);

static void proxyFree(
	void* pUserData,
	void* pMemory
);

static void proxyInternalAllocation(
	void* pUserData,
	size_t pSize,
	CtsSystemAllocationScope pScope
);

static void proxyInternalFree(
	void* pUserData,
	size_t pSize, 
	CtsSystemAllocationScope pScope
);

void ctsInitBumpAllocatorProxy(CtsBumpAllocator pInstance, CtsAllocationCallbacks* pAllocator) {
	pAllocator->userData           = pInstance;
	pAllocator->allocation         = proxyAllocation;
	pAllocator->reallocation       = proxyReallocation;
	pAllocator->free               = proxyFree;
	pAllocator->internalAllocation = proxyInternalAllocation;
	pAllocator->internalFree       = proxyInternalFree;
}

static void* proxyAllocation(
	void* pUserData,
	size_t pSize,
	size_t pAlign,
	CtsSystemAllocationScope pScope
) {
	(void) pScope;

	ctsBumpAllocatorAlloc((CtsBumpAllocator) pUserData, pSize, pAlign);
}

static void* proxyReallocation(
	void* pUserData,
	void* pOriginal,
	size_t pSize,
	size_t pAlign,
	CtsSystemAllocationScope pScope
) {
	(void) pScope;

	ctsBumpAllocatorRealloc((CtsBumpAllocator) pUserData, pOriginal, pSize, pAlign);
}

static void proxyFree(
	void* pUserData,
	void* pMemory
) {
	ctsBumpAllocatorFree((CtsBumpAllocator) pUserData, pMemory);
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