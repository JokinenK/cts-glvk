#include <stdbool.h>
#include <stddef.h>
#include <cts/descriptor_pool.h>
#include <cts/bump_allocator_proxy.h>
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

    CtsBumpAllocatorCreateInfo bumpAllocatorCreateInfo;
    bumpAllocatorCreateInfo.allocator = pAllocator;
    bumpAllocatorCreateInfo.growSize = 2ULL * 1024 * 1024; // 2Mb
    ctsCreateBumpAllocator(&descriptorPool->bumpAllocatorInstance, &bumpAllocatorCreateInfo);
    ctsInitBumpAllocatorProxy(descriptorPool->bumpAllocatorInstance, &descriptorPool->bumpAllocator);

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
        &pDescriptorPool->bumpAllocator,
        sizeof(struct CtsDescriptorSet),
        alignof(struct CtsDescriptorSet),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );
}

#ifdef __cplusplus
}
#endif