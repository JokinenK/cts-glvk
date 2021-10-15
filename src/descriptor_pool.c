#include <stdbool.h>
#include <stddef.h>
#include "cts/descriptor_pool.h"
#include "cts/private.h"
#include "cts/descriptor_pool_private.h"
#include "cts/descriptor_set_private.h"

#ifdef __cplusplus
extern "C" {
#endif

VkResult VKAPI_CALL ctsCreateDescriptorPool(
    VkDevice device,
    const VkDescriptorPoolCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDescriptorPool* pDescriptorPool
) {
    (void) device;

    struct CtsDescriptorPool* descriptorPool = ctsAllocation(
        pAllocator,
        sizeof(struct CtsDescriptorPool),
        alignof(struct CtsDescriptorPool),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (descriptorPool == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    set_loader_magic_value(descriptorPool);

    CtsLinearAllocatorCreateInfo linearAllocatorCreateInfo;
    linearAllocatorCreateInfo.pAllocator = pAllocator;
    linearAllocatorCreateInfo.growSize = 2ULL * 1024 * 1024; // 2Mb
    ctsCreateLinearAllocator(&descriptorPool->linearAllocator, &linearAllocatorCreateInfo);
    ctsGetLinearAllocatorCallbacks(descriptorPool->linearAllocator, &descriptorPool->allocator);

    *pDescriptorPool = CtsDescriptorPoolToHandle(descriptorPool);
    return VK_SUCCESS;
}

void VKAPI_CALL ctsDestroyDescriptorPool(
    VkDevice device,
    VkDescriptorPool descriptorPool,
    const VkAllocationCallbacks* pAllocator
) {
    if (descriptorPool != NULL) {
        ctsFree(pAllocator, descriptorPool);
    }
}

#ifdef __cplusplus
}
#endif