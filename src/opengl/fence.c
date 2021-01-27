#include <stddef.h>
#include <cts/fence.h>
#include <cts/commands.h>
#include <cts/platform_time.h>
#include <private/device_private.h>
#include <private/fence_private.h>
#include <private/queue_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateFence(
    CtsDevice device,
    const CtsFenceCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsFence* pFence
) {
    CtsResult result;
    CtsCreateFence cmd;
    cmd.base.type = CTS_COMMAND_CREATE_FENCE;
    cmd.base.pNext = NULL;

    cmd.device = device;
    cmd.pCreateInfo = pCreateInfo;
    cmd.pAllocator = pAllocator;
    cmd.pFence = pFence;
    cmd.pResult = &result;

    ctsQueueDispatch(device->queue, &cmd.base);

    return result;
}

CtsResult ctsResetFences(
    CtsDevice device,
    uint32_t fenceCount,
    const CtsFence* pFences
) {
    CtsResult result;
    CtsResetFences cmd;
    cmd.base.type = CTS_COMMAND_RESET_FENCES;
    cmd.base.pNext = NULL;

    cmd.device = device;
    cmd.fenceCount = fenceCount;
    cmd.pFences = pFences;
    cmd.pResult = &result;

    ctsQueueDispatch(device->queue, &cmd.base);

    return result;
}

CtsResult ctsGetFenceStatus(
    CtsDevice device,
    CtsFence fence
) {
    CtsResult result;
    CtsGetFenceStatus cmd;
    cmd.base.type = CTS_COMMAND_GET_FENCE_STATUS;
    cmd.base.pNext = NULL;

    cmd.device = device;
    cmd.fence = fence;
    cmd.pResult = &result;

    ctsQueueDispatch(device->queue, &cmd.base);

    return result;
}

void ctsSignalFence(
    CtsDevice device,
    CtsFence fence
) {
    CtsSignalFence cmd;
    cmd.base.type = CTS_COMMAND_SIGNAL_FENCE;
    cmd.base.pNext = NULL;

    cmd.device = device;
    cmd.fence = fence;

    ctsQueueDispatch(device->queue, &cmd.base);
}

CtsResult ctsWaitForFences(
    CtsDevice device,
    uint32_t fenceCount,
    const CtsFence* pFences,
    CtsBool32 waitAll,
    uint64_t timeout
) {
    CtsResult result;
    CtsWaitForFences cmd;
    cmd.base.type = CTS_COMMAND_WAIT_FOR_FENCES;
    cmd.base.pNext = NULL;

    cmd.device = device;
    cmd.fenceCount = fenceCount;
    cmd.pFences = pFences;
    cmd.waitAll = waitAll;
    cmd.timeout = timeout;
    cmd.pResult = &result;

    ctsQueueDispatch(device->queue, &cmd.base);

    return result;
}

void ctsDestroyFence(
    CtsDevice device,
    CtsFence fence,
    const CtsAllocationCallbacks* pAllocator
) {
    CtsDestroyFence cmd;
    cmd.base.type = CTS_COMMAND_DESTROY_FENCE;
    cmd.base.pNext = NULL;

    cmd.device = device;
    cmd.fence = fence;
    cmd.pAllocator = pAllocator;

    ctsQueueDispatch(device->queue, &cmd.base);
}

CtsResult ctsCreateFenceImpl(
    CtsDevice device,
    const CtsFenceCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsFence* pFence
) {
    (void) device;

    CtsFence fence = ctsAllocation(
        pAllocator,
        sizeof(struct CtsFenceImpl),
        alignof(struct CtsFenceImpl),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (fence == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    fence->sync = NULL;
    fence->status = (pCreateInfo->flags == CTS_FENCE_CREATE_SIGNALED_BIT)
        ? GL_SIGNALED
        : GL_UNSIGNALED;

    *pFence = fence;
    return CTS_SUCCESS;
}

CtsResult ctsResetFencesImpl(
    CtsDevice device,
    uint32_t fenceCount,
    const CtsFence* pFences
) {
    for (uint32_t i = 0; i < fenceCount; ++i) {
        CtsFence fence = pFences[i];

        if (fence->sync != NULL) {
            glDeleteSync(fence->sync);
        }

        fence->sync = NULL;
        fence->status = GL_UNSIGNALED;
    }

    return CTS_SUCCESS;
}

CtsResult ctsGetFenceStatusImpl(
    CtsDevice device,
    CtsFence fence
) {
    (void) device;

    if (fence->status == GL_SIGNALED) {
        return CTS_SUCCESS;
    }

    glGetSynciv(fence->sync, GL_SYNC_STATUS, sizeof(GLint), NULL, &fence->status);
    return (fence->status == GL_SIGNALED)
        ? CTS_SUCCESS
        : CTS_NOT_READY;
}

void ctsSignalFenceFenceImpl(
    CtsDevice device,
    CtsFence fence
) {
    if (fence->sync == NULL) {
        fence->sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    }
}

CtsResult ctsWaitForFencesImpl(
    CtsDevice device,
    uint32_t fenceCount,
    const CtsFence* pFences,
    CtsBool32 waitAll,
    uint64_t timeout
) {
    uint64_t begin = ctsGetCurrentTimeNs();
    uint64_t timeRemaining = timeout;
    uint32_t signaledCount;
    uint64_t timeoutPerFence;

    do {
        signaledCount = 0;
        timeRemaining = timeout - (begin - ctsGetCurrentTimeNs());
        timeoutPerFence = (uint64_t)timeRemaining / fenceCount;

        for (uint32_t i = 0; i < fenceCount; ++i) {
            if (pFences[i]->status == GL_SIGNALED) {
                ++signaledCount;
                continue;
            }
            
            if (pFences[i]->sync != NULL) {
                GLenum result = glClientWaitSync(pFences[i]->sync, 0, timeoutPerFence);

                if (result == GL_CONDITION_SATISFIED || result == GL_ALREADY_SIGNALED) {
                    pFences[i]->status = GL_SIGNALED;
                    ++signaledCount;
                }
            }
        }

        if (signaledCount == fenceCount || (!waitAll && signaledCount > 0)) {
            return CTS_SUCCESS;
        }
    } while (timeRemaining > 0);

    return CTS_TIMEOUT;
}

void ctsDestroyFenceImpl(
    CtsDevice device,
    CtsFence fence,
    const CtsAllocationCallbacks* pAllocator
) {
    (void) device;

    if (fence != NULL) {
        glDeleteSync(fence->sync);
        ctsFree(pAllocator, fence);
    }
}

#ifdef __cplusplus
}
#endif