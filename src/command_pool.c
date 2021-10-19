#include <stddef.h>
#include "cts/util/align.h"
#include "cts/allocator.h"
#include "cts/command_pool.h"
#include "cts/private.h"
#include "cts/command_pool_private.h"
#include "cts/command_buffer_private.h"

#ifdef __cplusplus
extern "C" {
#endif

VkResult VKAPI_CALL ctsCreateCommandPool(
    VkDevice device,
    const VkCommandPoolCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkCommandPool* pCommandPool
) {
    struct CtsCommandPool* commandPool = ctsAllocation(
        pAllocator,
        sizeof(struct CtsCommandPool),
        alignof(struct CtsCommandPool),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (commandPool == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    set_loader_magic_value(commandPool);

    CtsPoolAllocatorCreateInfo allocatorCreateInfo;
    allocatorCreateInfo.pAllocator = pAllocator;
    allocatorCreateInfo.growSize = 4 * 1024;
    allocatorCreateInfo.blockSize = 256;
    
    ctsCreatePoolAllocator(&commandPool->poolAllocator, &allocatorCreateInfo);
    ctsGetPoolAllocatorCallbacks(commandPool->poolAllocator, &commandPool->allocator);

    *pCommandPool = CtsCommandPoolToHandle(commandPool);
    return VK_SUCCESS;
}

void VKAPI_CALL ctsDestroyCommandPool(
    VkDevice device,
    VkCommandPool commandPoolHandle,
    const VkAllocationCallbacks* pAllocator
) {
    struct CtsCommandPool* commandPool = CtsCommandPoolFromHandle(commandPoolHandle);

    if (commandPool != NULL) {
        ctsDestroyPoolAllocator(commandPool->poolAllocator);
        ctsFree(pAllocator, commandPool);
    }
}


VkResult VKAPI_CALL ctsAllocateCommandBuffers(
    VkDevice deviceHandle,
    const VkCommandBufferAllocateInfo* pAllocateInfo,
    VkCommandBuffer* pCommandBuffers
) {
    VkResult result = VK_SUCCESS;
    uint32_t i = 0;

    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);
    struct CtsCommandPool* commandPool = CtsCommandPoolFromHandle(pAllocateInfo->commandPool);
    const VkAllocationCallbacks* pAllocator = &commandPool->allocator;

    for (; i < pAllocateInfo->commandBufferCount; ++i) {
        struct CtsCommandBuffer* commandBuffer = ctsAllocation(
            pAllocator,
            sizeof(struct CtsCommandBuffer),
            alignof(struct CtsCommandBuffer),
            VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
        );

        if (commandBuffer == NULL) {
            result = VK_ERROR_OUT_OF_HOST_MEMORY;
            break;
        }

        set_loader_magic_value(commandBuffer);

        commandBuffer->device = device;
        commandBuffer->pool = commandPool;
        commandBuffer->root = NULL;
        commandBuffer->current = NULL;
        commandBuffer->level = pAllocateInfo->level;
        commandBuffer->state = CTS_COMMAND_BUFFER_STATE_INITIAL;

        CtsLinearAllocatorCreateInfo allocatorCreateInfo;
        allocatorCreateInfo.growSize = 256;
        allocatorCreateInfo.pAllocator = pAllocator;

        ctsCreateLinearAllocator(&commandBuffer->linearAllocator, &allocatorCreateInfo);
        ctsGetLinearAllocatorCallbacks(commandBuffer->linearAllocator, &commandBuffer->allocator);

        pCommandBuffers[i] = CtsCommandBufferToHandle(commandBuffer);
    }

    for (; i < pAllocateInfo->commandBufferCount; ++i) {
        pCommandBuffers[i] = NULL;
    }

    return result;
}

void VKAPI_CALL ctsFreeCommandBuffers(
    VkDevice deviceHandle,
    VkCommandPool commandPoolHandle,
    uint32_t commandBufferCount,
    const VkCommandBuffer* pCommandBuffers
) {
    (void) deviceHandle;

    struct CtsCommandPool* commandPool = CtsCommandPoolFromHandle(commandPoolHandle);
    const VkAllocationCallbacks* pAllocator = &commandPool->allocator;

    for (uint32_t i = 0; i < commandBufferCount; ++i) {
        struct CtsCommandBuffer* commandBuffer = CtsCommandBufferFromHandle(pCommandBuffers[i]);
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