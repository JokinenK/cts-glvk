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

static bool waitForSurface(CtsQueue pQueue);
static void workerEntry(void* pArgs);

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

            CtsThreadCreateInfo threadCreateInfo;
            threadCreateInfo.entryPoint = workerEntry;
            threadCreateInfo.args = queue;
            ctsCreateThreads(&threadCreateInfo, pAllocator, 1, &queue->thread);

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
        ctsDestroyThread(pQueue->thread, pAllocator);
        ctsDestroyConditionVariable(pQueue->conditionVariable, pAllocator);
        ctsDestroyMutex(pQueue->mutex, pAllocator);

        ctsFree(pAllocator, pQueue->data);
        ctsFree(pAllocator, pQueue); 
       
        return true;
    }

    return false;
}

void ctsQueueDispatch(
    CtsQueue pQueue,
    const CtsCmdBase* pCmd,
    CtsSemaphore pSemaphore
) {
    CtsQueueItem queueItem;
    queueItem.cmd = pCmd;
    queueItem.semaphore = pSemaphore;

    ctsQueuePush(pQueue, &queueItem);
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

static bool waitForSurface(CtsQueue pQueue) {
    CtsDevice device = pQueue->device;
    CtsPhysicalDevice physicalDevice = device->physicalDevice;
    CtsInstance instance = physicalDevice->instance;

    while (device->isRunning && instance->surface == NULL) {
        ctsConditionVariableSleep(pQueue->conditionVariable, pQueue->mutex);
    }

    if (instance->surface != NULL) {
        ctsSurfaceMakeCurrent(instance->surface);
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
            cmd = cmd->next;
        }

        if (queueItem.semaphore != NULL) {
            ctsSignalSemaphores(1, &queueItem.semaphore);
        }
    }
}

#ifdef __cplusplus
}
#endif
