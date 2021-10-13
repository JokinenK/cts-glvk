#pragma once

#include <stdbool.h>
#include "vulkan/vulkan_core.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsGenericQueue {
    size_t size;
    size_t itemSize;

    size_t head;
    size_t tail;

    char* pData;
};

typedef struct CtsGenericQueue* CtsGenericQueue;

typedef struct CtsGenericQueueCreateInfo {
    size_t size;
    size_t itemSize;
} CtsGenericQueueCreateInfo;

VkResult ctsCreateGenericQueue(
    const CtsGenericQueueCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    CtsGenericQueue* pGenericQueue
);

void ctsDestroyGenericQueue(
    CtsGenericQueue genericQueue,
    const VkAllocationCallbacks* pAllocator
);

bool ctsGenericQueueEmpty(
    CtsGenericQueue genericQueue
);

bool ctsGenericQueueFull(
    CtsGenericQueue genericQueue
);

bool ctsGenericQueuePush(
    CtsGenericQueue genericQueue,
    const void* pData
);

bool ctsGenericQueuePop(
    CtsGenericQueue genericQueue,
    void* pData
);

#ifdef __cplusplus
}
#endif