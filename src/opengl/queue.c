#include "cts/util/align.h"
#include <stdio.h>
#include <string.h>
#include "cts/allocator.h"
#include <cts/command_dispatcher.h>
#include <private/private.h>
#include <private/device_private.h>
#include <private/instance_private.h>
#include <private/physical_device_private.h>
#include <private/queue_private.h>
#include <private/surface_private.h>

#ifdef __cplusplus
extern "C" {
#endif

static bool waitForSurface(struct CtsQueue* queue);
static void workerEntry(void* pArgs);

VkResult ctsCreateQueue(
    const CtsQueueCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    struct CtsQueue** ppQueue
) {
    struct CtsQueue* queue = ctsAllocation(
        pAllocator,
        sizeof(struct CtsQueue),
        alignof(struct CtsQueue),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (queue == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
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

    *ppQueue = queue;
    return VK_SUCCESS;
}


void ctsDestroyQueue(
    struct CtsQueue* queue,
    const VkAllocationCallbacks* pAllocator
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

VkResult ctsQueueWaitIdle(
    VkQueue queueHandle
) {
    struct CtsQueue* queue = CtsQueueFromHandle(queueHandle);

    ctsLockPlatformMutex(&queue->mutex);
    
    while (!ctsQueueEmpty(queue)) {
        ctsSleepPlatformConditionVariable(&queue->cmdFinishedCondVar, &queue->mutex);
    }

    ctsUnlockPlatformMutex(&queue->mutex);
    return VK_SUCCESS;
}

void ctsQueueDispatch(
    struct CtsQueue* queue,
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

bool ctsQueuePop(struct CtsQueue* queue, CtsQueueItem* pQueueItem) {
    ctsLockPlatformMutex(&queue->mutex);
    bool result = ctsGenericQueuePop(queue->queue, pQueueItem);
    ctsUnlockPlatformMutex(&queue->mutex);
    return result;
}


bool ctsQueueEmpty(struct CtsQueue* queue) {
    ctsLockPlatformMutex(&queue->mutex);
    bool result = ctsGenericQueueEmpty(queue->queue);
    ctsUnlockPlatformMutex(&queue->mutex);
    return result;
}

static void workerEntry(void* pArgs) {
    struct CtsQueue* queue = (struct CtsQueue*) pArgs;
    struct CtsPhysicalDevice* physicalDevice = queue->physicalDevice;

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
