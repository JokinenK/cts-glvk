#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <cts/commanddefs/cmd_base.h>
#include <cts/typedefs/queue.h>
#include <cts/typedefs/device.h>
#include <cts/allocator.h>
#include <cts/mutex.h>
#include <cts/thread.h>
#include <cts/generic_queue.h>
#include <cts/condition_variable.h>
#include <cts/queue.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsQueueCreateInfo {
    CtsDevice device;
    size_t size;
} CtsQueueCreateInfo;

typedef struct CtsQueueItem {
    const CtsCmdBase* cmd;
    volatile bool* pFinished;
} CtsQueueItem;

struct CtsQueueImpl {
    CtsDevice device;
    CtsThread thread;

    CtsGenericQueue queue;

    CtsMutex queueMutex;
    CtsMutex threadMutex;

    CtsConditionVariable queueCondVar;
    CtsConditionVariable threadCondVar;
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
    const CtsCmdBase* pCmd
);

bool ctsQueuePop(
    CtsQueue queue,
    CtsQueueItem* pQueueItem
);

bool ctsQueueEmpty(
    CtsQueue queue
);

#ifdef __cplusplus
}
#endif
