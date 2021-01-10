#include <cts/align.h>
#include <string.h>
#include <cts/allocator.h>
#include <private/queue_private.h>

#ifdef __cplusplus
extern "C" {
#endif

bool ctsCreateQueue(
    const CtsQueueCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    uint32_t pQueueCount,
    CtsQueue* pQueues
) {
    for (uint32_t i = 0; i < pQueueCount; ++i) {
        CtsQueue queue = ctsAllocation(
            pAllocator,
            sizeof(struct CtsQueue),
            alignof(struct CtsQueue),
            CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
        );

        if (queue) {
            queue->device = pCreateInfo->device;
            queue->head = 0;
            queue->tail = 0;
            queue->size = pCreateInfo->size;
            queue->data = ctsAllocation(
                pAllocator,
                sizeof(CtsQueueItem) * pCreateInfo->size,
                alignof(char),
                CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
            );

            CtsMutexCreateInfo mutexCreateInfo;
            ctsCreateMutexes(&mutexCreateInfo, pAllocator, 1, &queue->mutex);

            CtsConditionVariableCreateInfo conditionVariableCreateInfo;
            ctsCreateConditionVariables(&conditionVariableCreateInfo, pAllocator, 1, &queue->conditionVariable);

            pQueues[i] = queue;
        }
    }

    return true;
}

bool ctsDestroyQueue(
    CtsQueue pQueue,
    const CtsAllocationCallbacks* pAllocator
) {
    if (pQueue) {
        ctsConditionVariableWakeAll(pQueue->conditionVariable);
        ctsDestroyConditionVariable(pQueue->conditionVariable, pAllocator);

        ctsFree(pAllocator, pQueue->data);
        ctsFree(pAllocator, pQueue); 
       
        return true;
    }

    return false;
}

bool ctsQueuePush(CtsQueue pQueue, CtsQueueItem* pQueueItem) {
    ctsMutexLock(pQueue->mutex);

    if (((pQueue->head + 1) % pQueue->size) == pQueue->tail) {
        ctsMutexUnlock(pQueue->mutex);
        return false;
    }

    size_t idx = pQueue->head;
    pQueue->head = (pQueue->head + 1) % pQueue->size;
    pQueue->data[idx] = *pQueueItem;

    ctsMutexUnlock(pQueue->mutex);
    ctsConditionVariableWakeAll(pQueue->conditionVariable);
    return true;
}

bool ctsQueuePop(CtsQueue pQueue, CtsQueueItem* pQueueItem) {
    ctsMutexLock(pQueue->mutex);

    if (pQueue->tail == pQueue->head) {
        ctsMutexUnlock(pQueue->mutex);
        return false;
    }

    size_t idx = pQueue->tail;
    pQueue->tail = (pQueue->tail + 1) % pQueue->size;
    *pQueueItem = pQueue->data[idx];

    ctsMutexUnlock(pQueue->mutex);
    return true;
}

void ctsQueueWait(CtsQueue pQueue) {
    while (pQueue->tail == pQueue->head) {
      ctsConditionVariableSleep(pQueue->conditionVariable, pQueue->mutex);
    }
}

#ifdef __cplusplus
}
#endif
