#include <cts/align.h>
#include <cts/allocator.h>
#include <cts/semaphore.h>
#include <private/semaphore_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateSemaphore(
    CtsDevice device,
    const CtsSemaphoreCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsSemaphore* pSemaphore
) {
    CtsSemaphore semaphore = ctsAllocation(
        pAllocator,
        sizeof(struct CtsSemaphoreImpl),
        alignof(struct CtsSemaphoreImpl),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (semaphore == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    ctsInitPlatformSemaphore(&semaphore->semaphore, 0, 1);

    *pSemaphore = semaphore;
    return CTS_SUCCESS;
}

void ctsDestroySemaphore(
    CtsDevice device,
    CtsSemaphore semaphore,
    const CtsAllocationCallbacks* pAllocator
) {
    if (semaphore != NULL) {
        ctsDestroyPlatformSemaphore(&semaphore->semaphore);
        ctsFree(pAllocator, semaphore);
    }
}

CtsResult ctsWaitSemaphore(
    CtsSemaphore semaphore,
    uint64_t timeout
) {
    if (ctsWaitPlatformSemaphore(&semaphore->semaphore, timeout)) {
        return CTS_SUCCESS;
    }

    return CTS_TIMEOUT;
}

void ctsWaitSemaphores(
    uint32_t semaphoreCount,
    const CtsSemaphore* pSemaphores
) {
    for (uint32_t i = 0; i < semaphoreCount; ++i) {
        ctsWaitPlatformSemaphore(&pSemaphores[i]->semaphore, UINT64_MAX);
    }
}

void ctsSignalSemaphores(
    uint32_t semaphoreCount,
    const CtsSemaphore* pSemaphores
) {
    for (uint32_t i = 0; i < semaphoreCount; ++i) {
        ReleaseSemaphore(pSemaphores[i]->semaphore, 1, NULL);
    }
}

#ifdef __cplusplus
}
#endif