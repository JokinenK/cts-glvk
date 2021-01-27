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

    ctsInitPlatformMutex(&queue->queueMutex);
    ctsInitPlatformMutex(&queue->threadMutex);

    ctsInitPlatformConditionVariable(&queue->queueCondVar);
    ctsInitPlatformConditionVariable(&queue->threadCondVar);

    ctsInitPlatformThread(&queue->thread, workerEntry, queue);

    *pQueue = queue;
    return CTS_SUCCESS;
}


void ctsDestroyQueue(
    CtsQueue queue,
    const CtsAllocationCallbacks* pAllocator
) {
    if (queue) {
        ctsWakeAllPlatformConditionVariable(&queue->queueCondVar);
        ctsWakeAllPlatformConditionVariable(&queue->threadCondVar);
        
        ctsDestroyPlatformThread(&queue->thread);
        
        ctsDestroyPlatformConditionVariable(&queue->queueCondVar);
        ctsDestroyPlatformConditionVariable(&queue->threadCondVar);

        ctsDestroyPlatformMutex(&queue->queueMutex);
        ctsDestroyPlatformMutex(&queue->threadMutex);

        ctsDestroyGenericQueue(queue->queue, pAllocator);

        ctsFree(pAllocator, queue); 
    }
}

CtsResult ctsQueueWaitIdle(
    CtsQueue queue
) {
    ctsLockPlatformMutex(&queue->queueMutex);
    
    while (!ctsQueueEmpty(queue)) {
        ctsSleepPlatformConditionVariable(&queue->queueCondVar, &queue->queueMutex);
    }

    ctsUnlockPlatformMutex(&queue->queueMutex);
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

    ctsLockPlatformMutex(&queue->queueMutex);
    ctsGenericQueuePush(queue->queue, &queueItem);
    ctsWakeAllPlatformConditionVariable(&queue->threadCondVar);

    while (!finished) {
        ctsSleepPlatformConditionVariable(&queue->queueCondVar, &queue->queueMutex);
    }

    ctsUnlockPlatformMutex(&queue->queueMutex);
}

bool ctsQueuePop(CtsQueue queue, CtsQueueItem* pQueueItem) {
    ctsLockPlatformMutex(&queue->queueMutex);
    bool result = ctsGenericQueuePop(queue->queue, pQueueItem);
    ctsUnlockPlatformMutex(&queue->queueMutex);
    return result;
}


bool ctsQueueEmpty(CtsQueue queue) {
    ctsLockPlatformMutex(&queue->queueMutex);
    bool result = ctsGenericQueueEmpty(queue->queue);
    ctsUnlockPlatformMutex(&queue->queueMutex);
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
        ctsLockPlatformMutex(&queue->threadMutex);
        
        while (physicalDevice->isRunning && !ctsQueuePop(queue, &queueItem)) {
            ctsSleepPlatformConditionVariable(&queue->threadCondVar, &queue->threadMutex);
        }

        ctsUnlockPlatformMutex(&queue->threadMutex);

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

        ctsWakeAllPlatformConditionVariable(&queue->queueCondVar);
    }
}

#ifdef __cplusplus
}
#endif
