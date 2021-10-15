#include <stddef.h>
#include "cts/fence.h"
#include "cts/commands.h"
#include "cts/platform/platform_time.h"
#include "cts/private.h"
#include "cts/device_private.h"
#include "cts/fence_private.h"
#include "cts/queue_private.h"

#ifdef __cplusplus
extern "C" {
#endif

VkResult VKAPI_CALL ctsCreateFence(
    VkDevice deviceHandle,
    const VkFenceCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkFence* pFence
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);

    VkResult result;
    CtsCreateFence cmd;
    cmd.base.type = CTS_COMMAND_CREATE_FENCE;
    cmd.base.pNext = NULL;

    cmd.device = deviceHandle;
    cmd.pCreateInfo = pCreateInfo;
    cmd.pAllocator = pAllocator;
    cmd.pFence = pFence;
    cmd.pResult = &result;

    ctsQueueDispatch(device->queue, &cmd.base);
    return result;
}

VkResult VKAPI_CALL ctsResetFences(
    VkDevice deviceHandle,
    uint32_t fenceCount,
    const VkFence* pFences
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);

    VkResult result;
    CtsResetFences cmd;
    cmd.base.type = CTS_COMMAND_RESET_FENCES;
    cmd.base.pNext = NULL;

    cmd.device = deviceHandle;
    cmd.fenceCount = fenceCount;
    cmd.pFences = pFences;
    cmd.pResult = &result;

    ctsQueueDispatch(device->queue, &cmd.base);
    return result;
}

VkResult VKAPI_CALL ctsGetFenceStatus(
    VkDevice deviceHandle,
    VkFence fence
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);

    VkResult result;
    CtsGetFenceStatus cmd;
    cmd.base.type = CTS_COMMAND_GET_FENCE_STATUS;
    cmd.base.pNext = NULL;

    cmd.device = deviceHandle;
    cmd.fence = fence;
    cmd.pResult = &result;

    ctsQueueDispatch(device->queue, &cmd.base);
    return result;
}

void ctsSignalFence(
    VkDevice deviceHandle,
    VkFence fence
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);

    CtsSignalFence cmd;
    cmd.base.type = CTS_COMMAND_SIGNAL_FENCE;
    cmd.base.pNext = NULL;

    cmd.device = deviceHandle;
    cmd.fence = fence;

    ctsQueueDispatch(device->queue, &cmd.base);
}

VkResult VKAPI_CALL ctsWaitForFences(
    VkDevice deviceHandle,
    uint32_t fenceCount,
    const VkFence* pFences,
    VkBool32 waitAll,
    uint64_t timeout
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);

    VkResult result;
    CtsWaitForFences cmd;
    cmd.base.type = CTS_COMMAND_WAIT_FOR_FENCES;
    cmd.base.pNext = NULL;

    cmd.device = deviceHandle;
    cmd.fenceCount = fenceCount;
    cmd.pFences = pFences;
    cmd.waitAll = waitAll;
    cmd.timeout = timeout;
    cmd.pResult = &result;

    ctsQueueDispatch(device->queue, &cmd.base);
    return result;
}

void VKAPI_CALL ctsDestroyFence(
    VkDevice deviceHandle,
    VkFence fence,
    const VkAllocationCallbacks* pAllocator
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);

    CtsDestroyFence cmd;
    cmd.base.type = CTS_COMMAND_DESTROY_FENCE;
    cmd.base.pNext = NULL;

    cmd.device = deviceHandle;
    cmd.fence = fence;
    cmd.pAllocator = pAllocator;

    ctsQueueDispatch(device->queue, &cmd.base);
}

VkResult ctsCreateFenceImpl(
    VkDevice deviceHandle,
    const VkFenceCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkFence* pFence
) {
    (void) deviceHandle;

    struct CtsFence* fence = ctsAllocation(
        pAllocator,
        sizeof(struct CtsFence),
        alignof(struct CtsFence),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (fence == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    set_loader_magic_value(fence);

    fence->sync = NULL;
    fence->status = (pCreateInfo->flags == VK_FENCE_CREATE_SIGNALED_BIT)
        ? GL_SIGNALED
        : GL_UNSIGNALED;

    *pFence = CtsFenceToHandle(fence);
    return VK_SUCCESS;
}

VkResult ctsResetFencesImpl(
    VkDevice deviceHandle,
    uint32_t fenceCount,
    const VkFence* pFences
) {
    (void) deviceHandle;

    for (uint32_t i = 0; i < fenceCount; ++i) {
        struct CtsFence* fence = CtsFenceFromHandle(pFences[i]);

        if (fence->sync != NULL) {
            glDeleteSync(fence->sync);
        }

        fence->sync = NULL;
        fence->status = GL_UNSIGNALED;
    }

    return VK_SUCCESS;
}

VkResult ctsGetFenceStatusImpl(
    VkDevice deviceHandle,
    VkFence fenceHandle
) {
    (void) deviceHandle;

    struct CtsFence* fence = CtsFenceFromHandle(fenceHandle);

    if (fence->status == GL_SIGNALED) {
        return VK_SUCCESS;
    }

    glGetSynciv(fence->sync, GL_SYNC_STATUS, sizeof(GLint), NULL, &fence->status);
    return (fence->status == GL_SIGNALED)
        ? VK_SUCCESS
        : VK_NOT_READY;
}

void ctsSignalFenceImpl(
    VkDevice deviceHandle,
    VkFence fenceHandle
) {
    (void) deviceHandle;

    struct CtsFence* fence = CtsFenceFromHandle(fenceHandle);

    if (fence->sync == NULL) {
        fence->sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    }
}

VkResult VKAPI_CALL ctsWaitForFencesImpl(
    VkDevice deviceHandle,
    uint32_t fenceCount,
    const VkFence* pFences,
    VkBool32 waitAll,
    uint64_t timeout
) {
    (void) deviceHandle;

    uint64_t begin = ctsGetCurrentTimeNs();
    uint64_t timeRemaining = timeout;
    uint32_t signaledCount;
    uint64_t timeoutPerFence;

    do {
        signaledCount = 0;
        timeRemaining = timeout - (begin - ctsGetCurrentTimeNs());
        timeoutPerFence = (uint64_t)timeRemaining / fenceCount;

        for (uint32_t i = 0; i < fenceCount; ++i) {
            struct CtsFence* fence = CtsFenceFromHandle(pFences[i]);

            if (fence->status == GL_SIGNALED) {
                ++signaledCount;
                continue;
            }
            
            if (fence->sync != NULL) {
                GLenum result = glClientWaitSync(fence->sync, 0, timeoutPerFence);

                if (result == GL_CONDITION_SATISFIED || result == GL_ALREADY_SIGNALED) {
                    fence->status = GL_SIGNALED;
                    ++signaledCount;
                }
            }
        }

        if (signaledCount == fenceCount || (!waitAll && signaledCount > 0)) {
            return VK_SUCCESS;
        }
    } while (timeRemaining > 0);

    return VK_TIMEOUT;
}

void ctsDestroyFenceImpl(
    VkDevice deviceHandle,
    VkFence fenceHandle,
    const VkAllocationCallbacks* pAllocator
) {
    (void) deviceHandle;

    struct CtsFence* fence = CtsFenceFromHandle(fenceHandle);

    if (fence != NULL) {
        glDeleteSync(fence->sync);
        ctsFree(pAllocator, fence);
    }
}

#ifdef __cplusplus
}
#endif