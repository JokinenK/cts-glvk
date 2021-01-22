#include <stddef.h>
#include <cts/align.h>
#include <cts/allocator.h>
#include <cts/command_pool.h>
#include <cts/command_dispatcher.h>
#include <private/command_pool_private.h>
#include <private/command_buffer_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateCommandPool(
    CtsDevice device,
    const CtsCommandPoolCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsCommandPool* pCommandPool
) {
    CtsCommandPool commandPool = ctsAllocation(
        pAllocator,
        sizeof(struct CtsCommandPoolImpl),
        alignof(struct CtsCommandPoolImpl),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (commandPool == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    CtsPoolAllocatorCreateInfo allocatorCreateInfo;
    allocatorCreateInfo.pAllocator = pAllocator;
    allocatorCreateInfo.growSize = 4 * 1024;
    allocatorCreateInfo.blockSize = 256;
    
    ctsCreatePoolAllocator(&commandPool->poolAllocator, &allocatorCreateInfo);
    ctsGetPoolAllocatorCallbacks(commandPool->poolAllocator, &commandPool->allocator);

    *pCommandPool = commandPool;
    return CTS_SUCCESS;
}

void ctsDestroyCommandPool(
    CtsDevice device,
    CtsCommandPool commandPool,
    const CtsAllocationCallbacks* pAllocator
) {
    if (commandPool != NULL) {
        ctsDestroyPoolAllocator(commandPool->poolAllocator);
        ctsFree(pAllocator, commandPool);
    }
}


CtsResult ctsAllocateCommandBuffers(
    CtsDevice device,
    const CtsCommandBufferAllocateInfo* pAllocateInfo,
    CtsCommandBuffer* pCommandBuffers
) {
    CtsResult result = CTS_SUCCESS;
    uint32_t i = 0;

    CtsCommandPool commandPool = pAllocateInfo->commandPool;
    const CtsAllocationCallbacks* pAllocator = &commandPool->allocator;

    for (; i < pAllocateInfo->commandBufferCount; ++i) {
        CtsCommandBuffer commandBuffer = ctsAllocation(
            pAllocator,
            sizeof(struct CtsCommandBufferImpl),
            alignof(struct CtsCommandBufferImpl),
            CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
        );

        if (commandBuffer == NULL) {
            result = CTS_ERROR_OUT_OF_HOST_MEMORY;
            break;
        }

        commandBuffer->device = device;
        commandBuffer->pool = pAllocateInfo->commandPool;
        commandBuffer->root = NULL;
        commandBuffer->current = NULL;
        commandBuffer->level = pAllocateInfo->level;
        commandBuffer->state = CTS_COMMAND_BUFFER_STATE_INITIAL;

        CtsLinearAllocatorCreateInfo allocatorCreateInfo;
        allocatorCreateInfo.growSize = 256;
        allocatorCreateInfo.pAllocator = pAllocator;

        ctsCreateLinearAllocator(&commandBuffer->linearAllocator, &allocatorCreateInfo);
        ctsGetLinearAllocatorCallbacks(commandBuffer->linearAllocator, &commandBuffer->allocator);

        pCommandBuffers[i] = commandBuffer;
    }

    for (; i < pAllocateInfo->commandBufferCount; ++i) {
        pCommandBuffers[i] = NULL;
    }

    return result;
}

void ctsFreeCommandBuffers(
    CtsDevice device,
    CtsCommandPool commandPool,
    uint32_t commandBufferCount,
    const CtsCommandBuffer* pCommandBuffers
) {
    const CtsAllocationCallbacks* pAllocator = &commandPool->allocator;

    for (uint32_t i = 0; i < commandBufferCount; ++i) {
        CtsCommandBuffer commandBuffer = pCommandBuffers[i];
        commandBuffer->root = NULL;
        commandBuffer->current = NULL;
        commandBuffer->state = CTS_COMMAND_BUFFER_STATE_INVALID;

        ctsDestroyLinearAllocator(commandBuffer->linearAllocator);
        ctsFree(pAllocator, commandBuffer);
    }
}

#ifdef __cplusplus
}
#endif