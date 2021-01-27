#include <stddef.h>
#include <string.h>
#include <cts/align.h>
#include <cts/generic_queue.h>
#include <cts/platform_mutex.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsGenericQueueImpl {
    size_t size;
    size_t itemSize;

    size_t head;
    size_t tail;

    char* pData;
};

CtsResult ctsCreateGenericQueue(
    const CtsGenericQueueCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsGenericQueue* pGenericQueue
) {
    CtsGenericQueue genericQueue = ctsAllocation(
        pAllocator,
        sizeof(struct CtsGenericQueueImpl),
        alignof(struct CtsGenericQueueImpl),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (genericQueue == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    genericQueue->size = pCreateInfo->size;
    genericQueue->itemSize = pCreateInfo->itemSize;
    genericQueue->head = 0;
    genericQueue->tail = 0;
    genericQueue->pData = ctsAllocation(
        pAllocator,
        sizeof(char) * (pCreateInfo->itemSize * pCreateInfo->size),
        alignof(char),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    *pGenericQueue = genericQueue;
    return CTS_SUCCESS;
}

void ctsDestroyGenericQueue(
    CtsGenericQueue genericQueue,
    const CtsAllocationCallbacks* pAllocator
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
