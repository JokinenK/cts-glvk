#include <windows.h>
#include <climits>
#include <cts/align.h>
#include <cts/allocator.h>
#include <cts/semaphore.h>
#include <private/semaphore_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateSemaphores(
    CtsDevice device,
    const CtsSemaphoreCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsSemaphore* pSemaphore
) {
    CtsSemaphore semaphore = ctsAllocation(
        pAllocator,
        sizeof(struct CtsSemaphore),
        alignof(struct CtsSemaphore),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (semaphore == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    semaphore->semaphore = CreateSemaphoreA( 
        NULL,    // default security attributes
        (LONG)0, // initial count
        (LONG)1, // maximum count
        NULL     // unnamed semaphore
    );

    *pSemaphore = semaphore;
    return CTS_SUCCESS;
}

void ctsDestroySemaphore(
    CtsDevice device,
    CtsSemaphore semaphore,
    const CtsAllocationCallbacks* pAllocator
) {
    if (semaphore != NULL) {
        CloseHandle(semaphore->semaphore); 
        ctsFree(pAllocator, semaphore);
    }
}

void ctsWaitSemaphores(
    uint32_t semaphoreCount,
    const CtsSemaphore* pSemaphores
) {
    for (uint32_t i = 0; i < semaphoreCount; ++i) {
        WaitForSingleObject(pSemaphores[i]->semaphore, INFINITE);
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