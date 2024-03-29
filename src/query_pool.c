#include <stddef.h>
#include "cts/query_pool.h"
#include "cts/private.h"
#include "cts/query_pool_private.h"

#ifdef __cplusplus
extern "C" {
#endif

VkResult VKAPI_CALL ctsCreateQueryPool(
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

    set_loader_magic_value(queryPool);

    *pQueryPool = CtsQueryPoolToHandle(queryPool);
    return VK_SUCCESS;
}

void VKAPI_CALL ctsDestroyQueryPool(
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