#include <cts/align.h>
#include <stdio.h>
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
        sizeof(struct CtsQueueImpl),
        alignof(struct CtsQueueImpl),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (queue == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    queue->device = pCreateInfo->device;

    CtsGenericQueueCreateInfo genericQueueCreateInfo;
    genericQueueCreateInfo.size = pCreateInfo->size;
    genericQueueCreateInfo.itemSize = sizeof(CtsQueueItem);
    ctsCreateGenericQueue(&genericQueueCreateInfo, pAllocator, &queue->queue);

    CtsMutexCreateInfo mutexCreateInfo;
    ctsCreateMutex(&mutexCreateInfo, pAllocator, &queue->queueMutex);
    ctsCreateMutex(&mutexCreateInfo, pAllocator, &queue->threadMutex);

    CtsConditionVariableCreateInfo conditionVariableCreateInfo;
    ctsCreateConditionVariable(&conditionVariableCreateInfo, pAllocator, &queue->queueCondVar);
    ctsCreateConditionVariable(&conditionVariableCreateInfo, pAllocator, &queue->threadCondVar);

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
        ctsConditionVariableWakeAll(queue->queueCondVar);
        ctsConditionVariableWakeAll(queue->threadCondVar);
        
        ctsDestroyThread(queue->thread, pAllocator);
        
        ctsDestroyConditionVariable(queue->queueCondVar, pAllocator);
        ctsDestroyConditionVariable(queue->threadCondVar, pAllocator);

        ctsDestroyMutex(queue->queueMutex, pAllocator);
        ctsDestroyMutex(queue->threadMutex, pAllocator);

        ctsDestroyGenericQueue(queue->queue, pAllocator);

        ctsFree(pAllocator, queue); 
    }
}

CtsResult ctsQueueWaitIdle(
    CtsQueue queue
) {
    ctsMutexLock(queue->queueMutex);
    
    while (!ctsQueueEmpty(queue)) {
        ctsConditionVariableSleep(queue->queueCondVar, queue->queueMutex);
    }

    ctsMutexUnlock(queue->queueMutex);
    return CTS_SUCCESS;
}

void ctsQueueDispatch(
    CtsQueue queue,
    const CtsCmdBase* pCmd
) {
    volatile bool finished = false;
    CtsQueueItem queueItem;
    queueItem.cmd = pCmd;
    queueItem.pFinished = &finished;

    ctsMutexLock(queue->queueMutex);
    ctsGenericQueuePush(queue->queue, &queueItem);
    ctsConditionVariableWakeAll(queue->threadCondVar);

    while (!finished) {
        ctsConditionVariableSleep(queue->queueCondVar, queue->queueMutex);
    }

    ctsMutexUnlock(queue->queueMutex);
}

bool ctsQueuePop(CtsQueue queue, CtsQueueItem* pQueueItem) {
    ctsMutexLock(queue->queueMutex);
    bool result = ctsGenericQueuePop(queue->queue, pQueueItem);
    ctsMutexUnlock(queue->queueMutex);
    return result;
}


bool ctsQueueEmpty(CtsQueue queue) {
    ctsMutexLock(queue->queueMutex);
    bool result = ctsGenericQueueEmpty(queue->queue);
    ctsMutexUnlock(queue->queueMutex);
    return result;
}

static bool waitForSurface(CtsQueue queue) {
    CtsDevice device = queue->device;
    CtsPhysicalDevice physicalDevice = device->physicalDevice;

    ctsMutexLock(physicalDevice->mutex);

    while (device->isRunning && physicalDevice->surface == NULL) {
        ctsConditionVariableSleep(physicalDevice->conditionVariable, physicalDevice->mutex);
    }

    ctsMutexUnlock(physicalDevice->mutex);

    if (physicalDevice->surface != NULL) {
        ctsSurfaceMakeCurrent(physicalDevice->surface);
    }
    
    return (physicalDevice->surface != NULL);
}

static void workerEntry(void* pArgs) {
    CtsQueue queue = (CtsQueue) pArgs;
    CtsDevice device = queue->device;

    if (!waitForSurface(queue)) {
        return;
    }

    CtsQueueItem queueItem;
    const CtsCmdBase* cmd;
    const CtsCommandMetadata* commandMetadata;

    while (device->isRunning) {
        ctsMutexLock(queue->threadMutex);
        
        while (device->isRunning && !ctsQueuePop(queue, &queueItem)) {
            ctsConditionVariableSleep(queue->threadCondVar, queue->threadMutex);
        }

        ctsMutexUnlock(queue->threadMutex);

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

        ctsConditionVariableWakeAll(queue->queueCondVar);
    }
}

#ifdef __cplusplus
}
#endif
