#include <cts/align.h>
#include <pthread.h> 
#include <semaphore.h>
#include <cts/allocator.h>
#include <cts/os/semaphore.h>
#include <private/semaphore_private.h>

#ifdef __cplusplus
extern "C" {
#endif

bool ctsCreateSemaphores(
    const CtsSemaphoreCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    uint32_t pSemaphoreCount,
    CtsSemaphore* pSemaphores
) {
    for (uint32_t i = 0; i < pSemaphoreCount; ++i) {
        CtsSemaphore semaphore = ctsAllocation(
            pAllocator,
            sizeof(struct CtsSemaphore),
            alignof(struct CtsSemaphore),
            CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
        );

        if (!semaphore) {
            return false;
        }

        sem_init(&semaphore->semaphore, 0, pCreateInfo->initialValue); 
        pSemaphores[i] = semaphore;
    }

    return true;
}

bool ctsDestroySemaphore(
    CtsSemaphore pSemaphore,
    const CtsAllocationCallbacks* pAllocator
) {
    if (pSemaphore) {
        sem_destroy(&pSemaphore->semaphore); 
        ctsFree(pAllocator, pSemaphore);
        return true;
    }

    return false;
}

void ctsSemaphoreWait(
    CtsSemaphore pSemaphore
) {
    sem_wait(&pSemaphore->semaphore); 
}

bool ctsSemaphoreTryWait(
    CtsSemaphore pSemaphore
) {
    return (sem_trywait(&pSemaphore->semaphore) == 0);
}

void ctsSemaphorePost(
    CtsSemaphore pSemaphore
) {
    sem_post(&pSemaphore->semaphore);
}

#ifdef __cplusplus
}
#endif