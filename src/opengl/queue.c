#include <cts/align.h>
#include <string.h>
#include <cts/allocator.h>
#include <private/queue_private.h>

#ifdef __cplusplus
extern "C" {
#endif

bool ctsCreateQueue(
    const CtsQueueCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    uint32_t pQueueCount,
    CtsQueue* pQueues
) {
    for (uint32_t i = 0; i < pQueueCount; ++i) {
        CtsQueue queue = ctsAllocation(
            pAllocator,
            sizeof(struct CtsQueue),
            alignof(struct CtsQueue),
            CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
        );

        if (queue) {
            queue->device = pCreateInfo->device;
            queue->head = 0;
            queue->tail = 0;
            queue->size = pCreateInfo->size;
            queue->objSize = pCreateInfo->objSize;
            queue->buffer = ctsAllocation(
                pAllocator,
                sizeof(char) * (pCreateInfo->size * pCreateInfo->objSize),
                alignof(char),
                CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
            );

            pQueues[i] = queue;
        }
    }

    return true;
}

bool ctsDestroyQueue(
    CtsQueue pQueue,
    const CtsAllocationCallbacks* pAllocator
) {
    if (pQueue) {
        ctsFree(pAllocator, pQueue->buffer);
        ctsFree(pAllocator, pQueue); 
       
        return true;
    }

    return false;
}

bool ctsQueuePush(CtsQueue pQueue, void* pSrc, size_t pSrcLen) {
    if (((pQueue->head + 1) % pQueue->size) == pQueue->tail) {
        return false;
    }

    size_t offset = (pQueue->head * pQueue->objSize);
    pQueue->head = (pQueue->head + 1) % pQueue->size;

    void* dst = (char*)pQueue->buffer + offset;
    size_t dstLen = pQueue->objSize > pSrcLen ? pSrcLen : pQueue->objSize;

    mempcpy(dst, pSrc, dstLen);
    return true;
}

bool ctsQueuePop(CtsQueue pQueue, void* pDst, size_t pDstLen) {
    if (pQueue->tail == pQueue->head) {
        return false;
    }

    size_t offset = (pQueue->tail * pQueue->objSize);
    pQueue->tail = (pQueue->tail + 1) % pQueue->size;

    void* src = (char*)pQueue->buffer + offset;
    size_t srcLen = pQueue->objSize > pDstLen ? pDstLen : pQueue->objSize;

    mempcpy(pDst, src, srcLen);
    memset(src, 0, pQueue->objSize);
    return true;
}

#ifdef __cplusplus
}
#endif
