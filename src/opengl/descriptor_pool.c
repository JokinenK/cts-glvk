#include <stdbool.h>
#include <stddef.h>
#include <cts/descriptor_pool.h>
#include <private/descriptor_pool_private.h>
#include <private/descriptor_set_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateDescriptorPool(
    CtsDevice pDevice,
    const CtsDescriptorPoolCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsDescriptorPool* pDescriptorPool
) {
    (void) pDevice;

    CtsDescriptorPool descriptorPool = ctsAllocation(
        pAllocator,
        sizeof(struct CtsDescriptorPool),
        alignof(struct CtsDescriptorPool),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (descriptorPool == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    CtsLinearAllocatorCreateInfo linearAllocatorCreateInfo;
    linearAllocatorCreateInfo.allocator = pAllocator;
    linearAllocatorCreateInfo.growSize = 2ULL * 1024 * 1024; // 2Mb
    ctsCreateLinearAllocator(&descriptorPool->linearAllocatorInstance, &linearAllocatorCreateInfo);
    ctsGetLinearAllocatorCallbacks(descriptorPool->linearAllocatorInstance, &descriptorPool->linearAllocator);

    *pDescriptorPool = descriptorPool;
    return CTS_SUCCESS;
}

void ctsDestroyDescriptorPool(
    CtsDevice pDevice,
    CtsDescriptorPool pDescriptorPool,
    const CtsAllocationCallbacks* pAllocator
) {
    if (pDescriptorPool != NULL) {
        ctsFree(pAllocator, pDescriptorPool);
    }
}

CtsDescriptorSet ctsDescriptorPoolAllocateSet(
    CtsDescriptorPool pDescriptorPool
) {
    return ctsAllocation(
        &pDescriptorPool->linearAllocator,
        sizeof(struct CtsDescriptorSet),
        alignof(struct CtsDescriptorSet),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );
}

#ifdef __cplusplus
}
#endif