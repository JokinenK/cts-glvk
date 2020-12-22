#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <cts/typedefs/queue.h>
#include <cts/typedefs/device.h>
#include <cts/allocator.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsQueue {
    CtsDevice device;
    size_t head;
    size_t tail;
    size_t size;
    size_t objSize;
    void* buffer;
};

typedef struct CtsQueueCreateInfo {
    CtsDevice device;
    size_t size;
    size_t objSize;
} CtsQueueCreateInfo;

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
    void* pSrc,
    size_t pSrcLen
);

bool ctsQueuePop(
    CtsQueue pQueue,
    void* pDst,
    size_t pDstLen
);

#ifdef __cplusplus
}
#endif
