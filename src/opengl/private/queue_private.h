#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <cts/commanddefs/cmd_base.h>
#include <cts/typedefs/queue.h>
#include <cts/typedefs/device.h>
#include <cts/allocator.h>
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
    bool* pFinished;
    CtsConditionVariable conditionVariable;
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
    CtsQueue queue,
    const CtsAllocationCallbacks* pAllocator
);

void ctsQueueDispatch(
    CtsQueue queue,
    const CtsCmdBase* pCommand,
    CtsMutex mutex,
    CtsConditionVariable conditionVariable
);

bool ctsQueuePush(
    CtsQueue queue,
    CtsQueueItem* pQueueItem
);

bool ctsQueuePop(
    CtsQueue queue,
    CtsQueueItem* pQueueItem
);

#ifdef __cplusplus
}
#endif
