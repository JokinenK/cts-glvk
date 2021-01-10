#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <cts/commanddefs/cmd_base.h>
#include <cts/typedefs/queue.h>
#include <cts/typedefs/device.h>
#include <cts/allocator.h>
#include <cts/semaphore.h>
#include <cts/mutex.h>
#include <cts/condition_variable.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsQueueCreateInfo {
    CtsDevice device;
    size_t size;
} CtsQueueCreateInfo;

typedef struct CtsQueueItem {
    const CtsCmdBase* cmd;
    uint32_t semaphoreCount;
    CtsSemaphore* semaphores;
} CtsQueueItem;

struct CtsQueue {
    CtsDevice device;
    size_t head;
    size_t tail;
    size_t size;
    CtsQueueItem* data;
    CtsMutex mutex;
    CtsConditionVariable conditionVariable;
};

bool ctsCreateQueue(
    const CtsQueueCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    uint32_t pQueueCount,
    CtsQueue* pQueues
);

bool ctsDestroyQueue(
    CtsQueue pQueue,
    const CtsAllocationCallbacks* pAllocator
);

bool ctsQueuePush(
    CtsQueue pQueue,
    CtsQueueItem* pQueueItem
);

bool ctsQueuePop(
    CtsQueue pQueue,
    CtsQueueItem* pQueueItem
);

void ctsQueueWait(
    CtsQueue pQueue
);

#ifdef __cplusplus
}
#endif
