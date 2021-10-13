#include <stddef.h>
#include <cts/query_pool.h>
#include <private/private.h>
#include <private/query_pool_private.h>

#ifdef __cplusplus
extern "C" {
#endif

VkResult ctsCreateQueryPool(
    VkDevice deviceHandle,
    const VkQueryPoolCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkQueryPool* pQueryPool
) {
    (void) deviceHandle;

    struct CtsQueryPool* queryPool = ctsAllocation(
        pAllocator,
        sizeof(struct CtsQueryPool),
        alignof(struct CtsQueryPool),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (queryPool == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    *pQueryPool = CtsQueryPoolToHandle(queryPool);
    return VK_SUCCESS;
}

void ctsDestroyQueryPool(
    VkDevice deviceHandle,
    VkQueryPool queryPoolHandle,
    const VkAllocationCallbacks* pAllocator
) {
    (void) deviceHandle;

    struct CtsQueryPool* queryPool = CtsQueryPoolFromHandle(queryPoolHandle);

    if (queryPool != NULL) {
        ctsFree(pAllocator, queryPool);
    }
}

#ifdef __cplusplus
}
#endif