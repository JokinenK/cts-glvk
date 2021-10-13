#include <stddef.h>
#include <string.h>
#include "vulkan/vulkan_core.h"
#include "cts/allocator.h"
#include "cts/util/align.h"
#include "cts/util/generic_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

bool ctsInitGenericQueue(
    struct CtsGenericQueue* genericQueue,
    uint32_t itemSize,
    uint32_t size,
    const VkAllocationCallbacks* pAllocator
) {
    ctsInitPlatformMutex(&genericQueue->mutex);

    genericQueue->size = size;
    genericQueue->itemSize = itemSize;
    genericQueue->head = 0;
    genericQueue->tail = 0;
    genericQueue->pData = ctsAllocation(
        pAllocator,
        sizeof(char) * (itemSize * size),
        alignof(char),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    return true;
}

void ctsDestroyGenericQueue(
    struct CtsGenericQueue* genericQueue,
    const VkAllocationCallbacks* pAllocator
) {
    ctsDestroyPlatformMutex(&genericQueue->mutex);
    ctsFree(pAllocator, genericQueue->pData);
}

bool ctsGenericQueueEmpty(
    struct CtsGenericQueue* genericQueue
) {
    ctsLockPlatformMutex(&genericQueue->mutex);
    bool result = (genericQueue->tail == genericQueue->head);
    ctsUnlockPlatformMutex(&genericQueue->mutex);
    return result;
}

bool ctsGenericQueueFull(
    struct CtsGenericQueue* genericQueue
) {
    ctsLockPlatformMutex(&genericQueue->mutex);
    bool result = (((genericQueue->head + 1) % genericQueue->size) == genericQueue->tail);
    ctsUnlockPlatformMutex(&genericQueue->mutex);
    return result;
}

bool ctsGenericQueuePush(
    struct CtsGenericQueue* genericQueue,
    const void* pData
) {
    if (ctsGenericQueueFull(genericQueue)) {
        return false;
    }

    ctsLockPlatformMutex(&genericQueue->mutex);

    size_t idx = genericQueue->head;
    genericQueue->head = (genericQueue->head + 1) % genericQueue->size;

    size_t offset = (idx * genericQueue->itemSize);
    memcpy(&genericQueue->pData[offset], pData, genericQueue->itemSize);

    ctsUnlockPlatformMutex(&genericQueue->mutex);
    return true;
}

bool ctsGenericQueuePop(
    struct CtsGenericQueue* genericQueue,
    void* pData
) {
    if (ctsGenericQueueEmpty(genericQueue)) {
        return false;
    }

    ctsLockPlatformMutex(&genericQueue->mutex);

    size_t idx = genericQueue->tail;
    genericQueue->tail = (genericQueue->tail + 1) % genericQueue->size;

    size_t offset = (idx * genericQueue->itemSize);
    memcpy(pData, &genericQueue->pData[offset], genericQueue->itemSize);

    ctsUnlockPlatformMutex(&genericQueue->mutex);
    return true;
}

#ifdef __cplusplus
}
#endif
