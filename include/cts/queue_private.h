#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "vulkan/vulkan_core.h"
#include "cts/allocator.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/queue.h"
#include "cts/util/generic_queue.h"
#include "cts/platform/platform_condition_variable.h"
#include "cts/platform/platform_mutex.h"
#include "cts/platform/platform_thread.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsQueueCreateInfo {
    struct CtsPhysicalDevice* physicalDevice;
    uint32_t size;
} CtsQueueCreateInfo;

typedef struct CtsQueueItem {
    const CtsCmdBase* cmd;
    volatile bool* pFinished;
} CtsQueueItem;

struct CtsQueue {
    struct CtsObjectBase base;
    
    struct CtsPhysicalDevice* physicalDevice;
    CtsPlatformThread thread;
    
    CtsPlatformMutex mutex;
    CtsPlatformConditionVariable notEmptyCondition;
    CtsPlatformConditionVariable cmdFinishedCondVar;
    
    struct CtsGenericQueue queue;
};

VkResult ctsCreateQueue(
    const CtsQueueCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    struct CtsQueue** ppQueue
);

void ctsDestroyQueue(
    struct CtsQueue* queue,
    const VkAllocationCallbacks* pAllocator
);

void ctsQueueDispatch(
    struct CtsQueue* queue,
    const CtsCmdBase* pCmd
);

#ifdef __cplusplus
}
#endif
