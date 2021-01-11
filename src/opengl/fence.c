#include <stddef.h>
#include <cts/fence.h>
#include <cts/commands.h>
#include <cts/time.h>
#include <private/device_private.h>
#include <private/fence_private.h>
#include <private/queue_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateFence(
    CtsDevice pDevice,
    const CtsFenceCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsFence* pFence
) {
    CtsResult result;
    CtsCreateFence cmd;
    cmd.base.type = CTS_COMMAND_CREATE_FENCE;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.createInfo = pCreateInfo;
    cmd.allocator = pAllocator;
    cmd.fence = pFence;
    cmd.result = &result;

    ctsQueueDispatch(pDevice->queue, &cmd.base, pDevice->dispatchSemaphore);
    ctsWaitSemaphores(1, &pDevice->dispatchSemaphore);

    return result;
}

CtsResult ctsResetFences(
    CtsDevice pDevice,
    uint32_t pFenceCount,
    const CtsFence* pFences
) {
    CtsResult result;
    CtsResetFences cmd;
    cmd.base.type = CTS_COMMAND_RESET_FENCES;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.fenceCount = pFenceCount;
    cmd.fences = pFences;
    cmd.result = &result;

    ctsQueueDispatch(pDevice->queue, &cmd.base, pDevice->dispatchSemaphore);
    ctsWaitSemaphores(1, &pDevice->dispatchSemaphore);

    return result;
}

CtsResult ctsGetFenceStatus(
    CtsDevice pDevice,
    CtsFence pFence
) {
    CtsResult result;
    CtsGetFenceStatus cmd;
    cmd.base.type = CTS_COMMAND_GET_FENCE_STATUS;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.fence = pFence;
    cmd.result = &result;

    ctsQueueDispatch(pDevice->queue, &cmd.base, pDevice->dispatchSemaphore);
    ctsWaitSemaphores(1, &pDevice->dispatchSemaphore);

    return result;
}

CtsResult ctsWaitForFences(
    CtsDevice pDevice,
    uint32_t pFenceCount,
    const CtsFence* pFences,
    CtsBool32 pWaitAll,
    uint64_t pTimeout
) {
    CtsResult result;
    CtsWaitForFences cmd;
    cmd.base.type = CTS_COMMAND_WAIT_FOR_FENCES;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.fenceCount = pFenceCount;
    cmd.fences = pFences;
    cmd.waitAll = pWaitAll;
    cmd.timeout = pTimeout;
    cmd.result = &result;

    ctsQueueDispatch(pDevice->queue, &cmd.base, pDevice->dispatchSemaphore);
    ctsWaitSemaphores(1, &pDevice->dispatchSemaphore);

    return result;
}

void ctsDestroyFence(
    CtsDevice pDevice,
    CtsFence pFence,
    const CtsAllocationCallbacks* pAllocator
) {
    CtsDestroyFence cmd;
    cmd.base.type = CTS_COMMAND_DESTROY_FENCE;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.fence = pFence;
    cmd.allocator = pAllocator;

    ctsQueueDispatch(pDevice->queue, &cmd.base, pDevice->dispatchSemaphore);
    ctsWaitSemaphores(1, &pDevice->dispatchSemaphore);
}

CtsResult ctsCreateFenceImpl(
    CtsDevice pDevice,
    const CtsFenceCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsFence* pFence
) {
    (void) pDevice;

    CtsFence fence = ctsAllocation(
        pAllocator,
        sizeof(struct CtsFence),
        alignof(struct CtsFence),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (fence == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    fence->sync = (pCreateInfo->flags == CTS_FENCE_CREATE_SIGNALED_BIT)
        ? NULL
        : glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

    *pFence = fence;
    return CTS_SUCCESS;
}

CtsResult ctsResetFencesImpl(
    CtsDevice pDevice,
    uint32_t pFenceCount,
    const CtsFence* pFences
) {
    for (uint32_t i = 0; i < pFenceCount; ++i) {
        CtsFence fence = pFences[i];

        if (glIsSync(fence->sync)) {
            glDeleteSync(fence->sync);
        }

        fence->sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    }

    return CTS_SUCCESS;
}

CtsResult ctsGetFenceStatusImpl(
    CtsDevice pDevice,
    CtsFence pFence
) {
    (void) pDevice;

    if (!glIsSync(pFence->sync)) {
        return CTS_SUCCESS;
    }

    GLint status;
    glGetSynciv(pFence->sync, GL_SYNC_STATUS, sizeof(GLint), NULL, &status);

    return (status == GL_SIGNALED)
        ? CTS_SUCCESS
        : CTS_NOT_READY;
}

CtsResult ctsWaitForFencesImpl(
    CtsDevice pDevice,
    uint32_t pFenceCount,
    const CtsFence* pFences,
    CtsBool32 pWaitAll,
    uint64_t pTimeout
) {
    uint64_t begin = ctsGetCurrentTimeNs();
    int64_t timeRemaining = pTimeout;
    uint32_t signaledCount;
    uint64_t timeoutPerFence;

    do {
        signaledCount = 0;
        timeRemaining = pTimeout - (begin - ctsGetCurrentTimeNs());
        timeoutPerFence = (uint64_t)timeRemaining / pFenceCount;

        for (uint32_t i = 0; i < pFenceCount; ++i) {
            GLenum result = glClientWaitSync(pFences[i]->sync, 0, timeoutPerFence);
            if (result == GL_CONDITION_SATISFIED || result == GL_ALREADY_SIGNALED) {
                ++signaledCount;
            }
        }

        if (signaledCount == pFenceCount || (!pWaitAll && signaledCount > 0)) {
            return CTS_SUCCESS;
        }
    } while (timeRemaining > 0);

    return CTS_TIMEOUT;
}

void ctsDestroyFenceImpl(
    CtsDevice pDevice,
    CtsFence pFence,
    const CtsAllocationCallbacks* pAllocator
) {
    (void) pDevice;

    if (pFence != NULL) {
        if (glIsSync(pFence->sync)) {
            glDeleteSync(pFence->sync);
        }

        ctsFree(pAllocator, pFence);
    }
}

#ifdef __cplusplus
}
#endif