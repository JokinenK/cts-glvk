#include <stddef.h>
#include <cts/align.h>
#include <cts/allocator.h>
#include <cts/bump_allocator_proxy.h>
#include <cts/command_pool.h>
#include <private/command_pool_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateCommandPool(
    CtsDevice pDevice,
    const CtsCommandPoolCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsCommandPool* pCommandPool
) {
    CtsCommandPool commandPool = ctsAllocation(
        pAllocator,
        sizeof(struct CtsCommandPool),
        alignof(struct CtsCommandPool),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (commandPool == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    CtsBumpAllocatorCreateInfo bumpAllocatorCreateInfo;
    bumpAllocatorCreateInfo.allocator = pAllocator;
    bumpAllocatorCreateInfo.growSize = 2ULL * 1024 * 1024; // 2Mb
    ctsCreateBumpAllocator(&commandPool->bumpAllocatorInstance, &bumpAllocatorCreateInfo);
    ctsInitBumpAllocatorProxy(commandPool->bumpAllocatorInstance, &commandPool->bumpAllocator);

    *pCommandPool = commandPool;
    return CTS_SUCCESS;
}

void ctsDestroyCommandPool(
    CtsDevice pDevice,
    CtsCommandPool pCommandPool,
    const CtsAllocationCallbacks* pAllocator
) {
    if (pCommandPool != NULL) {
        ctsDestroyBumpAllocator(pCommandPool->bumpAllocatorInstance);
        ctsFree(pAllocator, pCommandPool);
    }
}

#ifdef __cplusplus
}
#endif