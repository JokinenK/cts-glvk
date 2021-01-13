#include <stddef.h>
#include <cts/query_pool.h>
#include <private/query_pool_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateQueryPool(
    CtsDevice device,
    const CtsQueryPoolCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsQueryPool* pQueryPool
) {
    CtsQueryPool queryPool = ctsAllocation(
        pAllocator,
        sizeof(CtsQueryPool),
        alignof(CtsQueryPool),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (queryPool == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    *pQueryPool = queryPool;
    return CTS_SUCCESS;
}

void ctsDestroyQueryPool(
    CtsDevice device,
    CtsQueryPool queryPool,
    const CtsAllocationCallbacks* pAllocator
) {
    if (queryPool != NULL) {
        ctsFree(pAllocator, queryPool);
    }
}

#ifdef __cplusplus
}
#endif