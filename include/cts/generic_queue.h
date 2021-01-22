#pragma once

#include <stdbool.h>
#include <cts/typedefs/enums.h>
#include <cts/allocator.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsGenericQueueImpl* CtsGenericQueue;

typedef struct CtsGenericQueueCreateInfo {
    size_t size;
    size_t itemSize;
} CtsGenericQueueCreateInfo;

CtsResult ctsCreateGenericQueue(
    const CtsGenericQueueCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsGenericQueue* pGenericQueue
);

void ctsDestroyGenericQueue(
    CtsGenericQueue genericQueue,
    const CtsAllocationCallbacks* pAllocator
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