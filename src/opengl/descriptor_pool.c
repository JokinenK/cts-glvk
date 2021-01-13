#include <stdbool.h>
#include <stddef.h>
#include <cts/descriptor_pool.h>
#include <private/descriptor_pool_private.h>
#include <private/descriptor_set_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateDescriptorPool(
    CtsDevice device,
    const CtsDescriptorPoolCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsDescriptorPool* pDescriptorPool
) {
    (void) device;

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
    linearAllocatorCreateInfo.pAllocator = pAllocator;
    linearAllocatorCreateInfo.growSize = 2ULL * 1024 * 1024; // 2Mb
    ctsCreateLinearAllocator(&descriptorPool->linearAllocatorInstance, &linearAllocatorCreateInfo);
    ctsGetLinearAllocatorCallbacks(descriptorPool->linearAllocatorInstance, &descriptorPool->linearAllocator);

    *pDescriptorPool = descriptorPool;
    return CTS_SUCCESS;
}

void ctsDestroyDescriptorPool(
    CtsDevice device,
    CtsDescriptorPool descriptorPool,
    const CtsAllocationCallbacks* pAllocator
) {
    if (descriptorPool != NULL) {
        ctsFree(pAllocator, descriptorPool);
    }
}

CtsDescriptorSet ctsDescriptorPoolAllocateSet(
    CtsDescriptorPool descriptorPool
) {
    return ctsAllocation(
        &descriptorPool->linearAllocator,
        sizeof(struct CtsDescriptorSet),
        alignof(struct CtsDescriptorSet),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );
}

#ifdef __cplusplus
}
#endif