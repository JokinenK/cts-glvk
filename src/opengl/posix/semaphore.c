#include <cts/align.h>
#include <pthread.h> 
#include <semaphore.h>
#include <cts/allocator.h>
#include <cts/semaphore.h>
#include <private/semaphore_private.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SEMAPHORE_MAX_VALUE 1

CtsResult ctsCreateSemaphore(
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

    if (!semaphore) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    sem_init(&semaphore->semaphore, 0, 0); 
    pSemaphore = semaphore;

    return CTS_SUCCESS;
}

void ctsDestroySemaphore(
    CtsDevice device,
    CtsSemaphore semaphore,
    const CtsAllocationCallbacks* pAllocator
) {
    if (pSemaphore != NULL) {
        sem_destroy(&pSemaphore->semaphore); 
        ctsFree(pAllocator, pSemaphore);
    }
}

void ctsWaitSemaphores(
    uint32_t semaphoreCount,
    const CtsSemaphore* pSemaphores
) {
    for (uint32_t i = 0; i < semaphoreCount; ++i) {
        sem_wait(&pSemaphores[i]->semaphore); 
    }
}

void ctsSignalSemaphores(
    uint32_t semaphoreCount,
    const CtsSemaphore* pSemaphores
) {
    int value;

    for (uint32_t i = 0; i < semaphoreCount; ++i) {
        sem_getvalue(&pSemaphore->semaphore, &value);

        if (value < SEMAPHORE_MAX_VALUE) {
            sem_post(&pSemaphore->semaphore); 
        }
    }
}

#ifdef __cplusplus
}
#endif