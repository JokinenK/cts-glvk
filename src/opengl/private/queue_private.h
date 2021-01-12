#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <cts/commanddefs/cmd_base.h>
#include <cts/typedefs/queue.h>
#include <cts/typedefs/device.h>
#include <cts/allocator.h>
#include <cts/semaphore.h>
#include <cts/mutex.h>
#include <cts/thread.h>
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
    CtsSemaphore semaphore;
} CtsQueueItem;

struct CtsQueue {
    CtsDevice device;
    CtsThread thread;
    size_t head;
    size_t tail;
    size_t size;
    CtsQueueItem* data;
    CtsMutex mutex;
    CtsConditionVariable conditionVariable;
};

CtsResult ctsCreateQueue(
    const CtsQueueCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsQueue* pQueue
);

void ctsDestroyQueue(
    CtsQueue pQueue,
    const CtsAllocationCallbacks* pAllocator
);

void ctsQueueDispatch(
    CtsQueue pQueue,
    const CtsCmdBase* pCommand,
    CtsSemaphore pSemaphore
);

bool ctsQueuePush(
    CtsQueue pQueue,
    CtsQueueItem* pQueueItem
);

bool ctsQueuePop(
    CtsQueue pQueue,
    CtsQueueItem* pQueueItem
);

#ifdef __cplusplus
}
#endif
