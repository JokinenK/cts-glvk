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

    queue->physicalDevice = pCreateInfo->physicalDevice;

    CtsGenericQueueCreateInfo genericQueueCreateInfo;
    genericQueueCreateInfo.size = pCreateInfo->size;
    genericQueueCreateInfo.itemSize = sizeof(CtsQueueItem);
    ctsCreateGenericQueue(&genericQueueCreateInfo, pAllocator, &queue->queue);

    ctsInitPlatformMutex(&queue->mutex);
    ctsInitPlatformConditionVariable(&queue->cmdFinishedCondVar);
    ctsInitPlatformConditionVariable(&queue->notEmptyCondition);

    ctsInitPlatformThread(&queue->thread, workerEntry, queue);

    *pQueue = queue;
    return CTS_SUCCESS;
}


void ctsDestroyQueue(
    CtsQueue queue,
    const CtsAllocationCallbacks* pAllocator
) {
    if (queue) {
        ctsWakeAllPlatformConditionVariable(&queue->cmdFinishedCondVar);
        ctsWakeAllPlatformConditionVariable(&queue->notEmptyCondition);
        
        ctsDestroyPlatformThread(&queue->thread);
        
        ctsDestroyPlatformConditionVariable(&queue->cmdFinishedCondVar);
        ctsDestroyPlatformConditionVariable(&queue->notEmptyCondition);

        ctsDestroyPlatformMutex(&queue->mutex);

        ctsDestroyGenericQueue(queue->queue, pAllocator);

        ctsFree(pAllocator, queue); 
    }
}

CtsResult ctsQueueWaitIdle(
    CtsQueue queue
) {
    ctsLockPlatformMutex(&queue->mutex);
    
    while (!ctsQueueEmpty(queue)) {
        ctsSleepPlatformConditionVariable(&queue->cmdFinishedCondVar, &queue->mutex);
    }

    ctsUnlockPlatformMutex(&queue->mutex);
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

    ctsLockPlatformMutex(&queue->mutex);
    ctsGenericQueuePush(queue->queue, &queueItem);
    ctsWakeAllPlatformConditionVariable(&queue->notEmptyCondition);

    while (!finished) {
        ctsSleepPlatformConditionVariable(&queue->cmdFinishedCondVar, &queue->mutex);
    }

    ctsUnlockPlatformMutex(&queue->mutex);
}

bool ctsQueuePop(CtsQueue queue, CtsQueueItem* pQueueItem) {
    ctsLockPlatformMutex(&queue->mutex);
    bool result = ctsGenericQueuePop(queue->queue, pQueueItem);
    ctsUnlockPlatformMutex(&queue->mutex);
    return result;
}


bool ctsQueueEmpty(CtsQueue queue) {
    ctsLockPlatformMutex(&queue->mutex);
    bool result = ctsGenericQueueEmpty(queue->queue);
    ctsUnlockPlatformMutex(&queue->mutex);
    return result;
}

static void workerEntry(void* pArgs) {
    CtsQueue queue = (CtsQueue) pArgs;
    CtsPhysicalDevice physicalDevice = queue->physicalDevice;

    if (!ctsPhysicalDeviceWaitSurface(physicalDevice)) {
        return;
    }

    CtsQueueItem queueItem;
    const CtsCmdBase* cmd;
    const CtsCommandMetadata* commandMetadata;

    while (physicalDevice->isRunning) {
        ctsLockPlatformMutex(&queue->mutex);
        
        while (physicalDevice->isRunning && !ctsQueuePop(queue, &queueItem)) {
            ctsSleepPlatformConditionVariable(&queue->notEmptyCondition, &queue->mutex);
        }

        ctsUnlockPlatformMutex(&queue->mutex);

        if (!physicalDevice->isRunning) {
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

        ctsWakeAllPlatformConditionVariable(&queue->cmdFinishedCondVar);
    }
}

#ifdef __cplusplus
}
#endif
