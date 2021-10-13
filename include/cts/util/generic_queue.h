#pragma once

#include <stdbool.h>
#include "vulkan/vulkan_core.h"
#include "cts/platform/platform_mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsGenericQueue {
    size_t size;
    size_t itemSize;

    size_t head;
    size_t tail;

    char* pData;
    CtsPlatformMutex mutex;
};

bool ctsInitGenericQueue(
    struct CtsGenericQueue* pGenericQueue,
    uint32_t itemSize,
    uint32_t size,
    const VkAllocationCallbacks* pAllocator
);

void ctsDestroyGenericQueue(
    struct CtsGenericQueue* genericQueue,
    const VkAllocationCallbacks* pAllocator
);

bool ctsGenericQueueEmpty(
    struct CtsGenericQueue* genericQueue
);

bool ctsGenericQueueFull(
    struct CtsGenericQueue* genericQueue
);

bool ctsGenericQueuePush(
    struct CtsGenericQueue* genericQueue,
    const void* pData
);

bool ctsGenericQueuePop(
    struct CtsGenericQueue* genericQueue,
    void* pData
);

#ifdef __cplusplus
}
#endif