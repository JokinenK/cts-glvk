#include <stdio.h>
#include <string.h>
#include "cts/util/align.h"
#include "cts/allocator.h"
#include "cts/command_dispatcher.h"
#include "cts/private.h"
#include "cts/device_private.h"
#include "cts/instance_private.h"
#include "cts/physical_device_private.h"
#include "cts/queue_private.h"
#include "cts/surface_private.h"

#ifdef __cplusplus
extern "C" {
#endif

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

    ctsInitGenericQueue(&queue->queue, sizeof(CtsQueueItem), pCreateInfo->size, pAllocator);

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

        ctsDestroyGenericQueue(&queue->queue, pAllocator);

        ctsFree(pAllocator, queue); 
    }
}

VkResult VKAPI_CALL ctsQueueWaitIdle(
    VkQueue queueHandle
) {
    struct CtsQueue* queue = CtsQueueFromHandle(queueHandle);

    ctsLockPlatformMutex(&queue->mutex);
    
    while (!ctsGenericQueueEmpty(&queue->queue)) {
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

    while (!ctsGenericQueuePush(&queue->queue, &queueItem)) {
        ctsSleepPlatformConditionVariable(&queue->cmdFinishedCondVar, &queue->mutex);
    }

    ctsWakeAllPlatformConditionVariable(&queue->notEmptyCondition);

    while (!finished) {
        ctsSleepPlatformConditionVariable(&queue->cmdFinishedCondVar, &queue->mutex);
    }

    ctsUnlockPlatformMutex(&queue->mutex);
}

static void workerEntry(void* pArgs) {
    struct CtsQueue* queue = (struct CtsQueue*) pArgs;
    struct CtsPhysicalDevice* physicalDevice = queue->physicalDevice;
    struct CtsGlContext* context = &physicalDevice->offscreenContext;    

    CtsQueueItem queueItem;
    const CtsCmdBase* cmd;
    const CtsCommandMetadata* commandMetadata;

    while (physicalDevice->isRunning) {
        ctsLockPlatformMutex(&queue->mutex);
        
        while (physicalDevice->isRunning && physicalDevice->surface == NULL) {
            ctsSleepPlatformConditionVariable(&queue->surfacePresentCondVar, &queue->mutex);
        }
        
        while (physicalDevice->isRunning && !ctsGenericQueuePop(&queue->queue, &queueItem)) {
            ctsSleepPlatformConditionVariable(&queue->notEmptyCondition, &queue->mutex);
        }

        ctsUnlockPlatformMutex(&queue->mutex);
        ctsGlContextMakeCurrent(ctsSurfaceGetGlContext(physicalDevice->surface));

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
