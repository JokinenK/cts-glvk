#include <cts/align.h>
#include <string.h>
#include <cts/allocator.h>
#include <cts/command_dispatcher.h>
#include <private/device_private.h>
#include <private/instance_private.h>
#include <private/physical_device_private.h>
#include <private/queue_private.h>
#include <private/surface_private.h>

#ifdef __cplusplus
extern "C" {
#endif

static bool waitForSurface(CtsQueue queue);
static void workerEntry(void* pArgs);

CtsResult ctsCreateQueue(
    const CtsQueueCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsQueue* pQueue
) {
    CtsQueue queue = ctsAllocation(
        pAllocator,
        sizeof(struct CtsQueue),
        alignof(struct CtsQueue),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (queue == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

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
    ctsCreateConditionVariable(&conditionVariableCreateInfo, pAllocator, &queue->conditionVariable);

    CtsThreadCreateInfo threadCreateInfo;
    threadCreateInfo.pfEntryPoint = workerEntry;
    threadCreateInfo.pArgs = queue;
    ctsCreateThread(&threadCreateInfo, pAllocator, &queue->thread);

    *pQueue = queue;
    return CTS_SUCCESS;
}

void ctsDestroyQueue(
    CtsQueue queue,
    const CtsAllocationCallbacks* pAllocator
) {
    if (queue) {
        ctsConditionVariableWakeAll(queue->conditionVariable);
        ctsDestroyThread(queue->thread, pAllocator);
        ctsDestroyConditionVariable(queue->conditionVariable, pAllocator);
        ctsDestroyMutex(queue->mutex, pAllocator);

        ctsFree(pAllocator, queue->data);
        ctsFree(pAllocator, queue); 
    }
}

void ctsQueueDispatch(
    CtsQueue queue,
    const CtsCmdBase* pCmd,
    CtsMutex mutex,
    CtsConditionVariable conditionVariable
) {
    bool wait = (conditionVariable != NULL);
    bool finished = false;

    CtsQueueItem queueItem;
    queueItem.cmd = pCmd;
    queueItem.pFinished = (wait ? &finished : NULL);
    queueItem.conditionVariable = conditionVariable;

    ctsQueuePush(queue, &queueItem);

    while (wait && !finished) {
        ctsConditionVariableSleep(conditionVariable, mutex);
    }
}

bool ctsQueuePush(CtsQueue queue, CtsQueueItem* pQueueItem) {
    ctsMutexLock(queue->mutex);

    if (((queue->head + 1) % queue->size) == queue->tail) {
        ctsMutexUnlock(queue->mutex);
        return false;
    }

    size_t idx = queue->head;
    queue->head = (queue->head + 1) % queue->size;
    queue->data[idx] = *pQueueItem;

    ctsMutexUnlock(queue->mutex);
    ctsConditionVariableWakeAll(queue->conditionVariable);
    return true;
}

bool ctsQueuePop(CtsQueue queue, CtsQueueItem* pQueueItem) {
    ctsMutexLock(queue->mutex);

    if (queue->tail == queue->head) {
        ctsMutexUnlock(queue->mutex);
        return false;
    }

    size_t idx = queue->tail;
    queue->tail = (queue->tail + 1) % queue->size;
    *pQueueItem = queue->data[idx];

    ctsMutexUnlock(queue->mutex);
    return true;
}

static bool waitForSurface(CtsQueue queue) {
    CtsDevice device = queue->device;

    while (device->isRunning && device->surface == NULL) {
        ctsConditionVariableSleep(queue->conditionVariable, queue->mutex);
    }

    if (device->surface != NULL) {
        ctsSurfaceMakeCurrent(device->surface);
        return true;
    }
    
    return false;   
}

static void workerEntry(void* pArgs) {
    CtsQueue queue = (CtsQueue) pArgs;
    CtsDevice device = queue->device;

    if (!waitForSurface(queue)) {
        return;
    }

    CtsQueueItem queueItem;
    const CtsCommandMetadata* commandMetadata;
    const CtsCmdBase* cmd;

    while (device->isRunning) {
        while (device->isRunning && !ctsQueuePop(queue, &queueItem)) {
            ctsConditionVariableSleep(queue->conditionVariable, queue->mutex);
        }

        if (!device->isRunning) {
            break;
        }

        cmd = queueItem.cmd;
        while (cmd != NULL) {
            commandMetadata = ctsGetCommandMetadata(cmd->type);
            commandMetadata->handler(cmd);
            cmd = cmd->pNext;
        }

        if (queueItem.pFinished != NULL) {
            *queueItem.pFinished = true;
        }

        if (queueItem.conditionVariable != NULL) {
            ctsConditionVariableWakeAll(queueItem.conditionVariable);
        }
    }
}

#ifdef __cplusplus
}
#endif
