#include <stddef.h>
#include <string.h>
#include "vulkan/vulkan_core.h"
#include "cts/allocator.h"
#include "cts/util/align.h"
#include "cts/util/generic_queue.h"
#include "cts/platform/platform_mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

VkResult ctsCreateGenericQueue(
    const CtsGenericQueueCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    CtsGenericQueue* pGenericQueue
) {
    struct CtsGenericQueue* genericQueue = ctsAllocation(
        pAllocator,
        sizeof(struct CtsGenericQueue),
        alignof(struct CtsGenericQueue),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (genericQueue == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    genericQueue->size = pCreateInfo->size;
    genericQueue->itemSize = pCreateInfo->itemSize;
    genericQueue->head = 0;
    genericQueue->tail = 0;
    genericQueue->pData = ctsAllocation(
        pAllocator,
        sizeof(char) * (pCreateInfo->itemSize * pCreateInfo->size),
        alignof(char),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    *pGenericQueue = genericQueue;
    return VK_SUCCESS;
}

void ctsDestroyGenericQueue(
    CtsGenericQueue genericQueue,
    const VkAllocationCallbacks* pAllocator
) {
    if (genericQueue) {
        ctsFree(pAllocator, genericQueue->pData);
        ctsFree(pAllocator, genericQueue); 
    }
}

bool ctsGenericQueueEmpty(
    CtsGenericQueue genericQueue
) {
    return (genericQueue->tail == genericQueue->head);
}

bool ctsGenericQueueFull(
    CtsGenericQueue genericQueue
) {
    return (((genericQueue->head + 1) % genericQueue->size) == genericQueue->tail);
}

bool ctsGenericQueuePush(
    CtsGenericQueue genericQueue,
    const void* pData
) {
    if (ctsGenericQueueFull(genericQueue)) {
        return false;
    }

    size_t idx = genericQueue->head;
    genericQueue->head = (genericQueue->head + 1) % genericQueue->size;

    size_t offset = (idx * genericQueue->itemSize);
    memcpy(&genericQueue->pData[offset], pData, genericQueue->itemSize);
    
    return true;
}

bool ctsGenericQueuePop(
    CtsGenericQueue genericQueue,
    void* pData
) {
    if (ctsGenericQueueEmpty(genericQueue)) {
        return false;
    }

    size_t idx = genericQueue->tail;
    genericQueue->tail = (genericQueue->tail + 1) % genericQueue->size;

    size_t offset = (idx * genericQueue->itemSize);
    memcpy(pData, &genericQueue->pData[offset], genericQueue->itemSize);

    return true;
}

#ifdef __cplusplus
}
#endif
