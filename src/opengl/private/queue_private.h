#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <cts/commanddefs/cmd_base.h>
#include <cts/typedefs/queue.h>
#include <cts/typedefs/device.h>
#include <cts/allocator.h>
#include <cts/generic_queue.h>
#include <cts/queue.h>
#include <cts/platform_condition_variable.h>
#include <cts/platform_mutex.h>
#include <cts/platform_thread.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsQueueCreateInfo {
    CtsPhysicalDevice physicalDevice;
    size_t size;
} CtsQueueCreateInfo;

typedef struct CtsQueueItem {
    const CtsCmdBase* cmd;
    volatile bool* pFinished;
} CtsQueueItem;

struct CtsQueueImpl {
    CtsPhysicalDevice physicalDevice;
    CtsPlatformThread thread;
    
    CtsPlatformMutex threadMutex;
    CtsPlatformMutex queueMutex;
    
    CtsPlatformConditionVariable threadCondVar;
    CtsPlatformConditionVariable queueCondVar;
    
    CtsGenericQueue queue;
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
