#include <stddef.h>
#include <cts/align.h>
#include <cts/allocator.h>
#include <cts/command_pool.h>
#include <cts/command_dispatcher.h>
#include <private/command_pool_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateCommandPool(
    CtsDevice device,
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

    CtsPoolAllocatorCreateInfo poolAllocatorCreateInfo;
    poolAllocatorCreateInfo.pAllocator = pAllocator;
    poolAllocatorCreateInfo.growSize = 2ULL * 1024 * 1024; // 2Mb
    poolAllocatorCreateInfo.blockSize = ctsGetMaxCommandSize();
    ctsCreatePoolAllocator(&commandPool->poolAllocatorInstance, &poolAllocatorCreateInfo);
    ctsGetPoolAllocatorCallbacks(commandPool->poolAllocatorInstance, &commandPool->poolAllocator);

    *pCommandPool = commandPool;
    return CTS_SUCCESS;
}

void ctsDestroyCommandPool(
    CtsDevice device,
    CtsCommandPool commandPool,
    const CtsAllocationCallbacks* pAllocator
) {
    if (commandPool != NULL) {
        ctsDestroyPoolAllocator(commandPool->poolAllocatorInstance);
        ctsFree(pAllocator, commandPool);
    }
}

#ifdef __cplusplus
}
#endif