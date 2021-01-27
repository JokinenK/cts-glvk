#include <cts/platform_semaphore.h>

#ifdef __cplusplus
extern "C" {
#endif

void ctsInitPlatformSemaphore(
    CtsPlatformSemaphore* pSemaphore,
    uint32_t initialValue,
    uint32_t maxValue
) {
    pSemaphore->maxValue = maxValue;
    sem_init(&pSemaphore->semaphore, 0, 0); 
}

void ctsDestroyPlatformSemaphore(
    CtsPlatformSemaphore* pSemaphore
) {
    sem_destroy(&pSemaphore->semaphore); 
}

bool ctsWaitPlatformSemaphore(
    CtsPlatformSemaphore* pSemaphore,
    uint64_t timeout
) {
    // TODO: Handle this
    //struct timespec ts;
    //sem_timedwait(&pSemaphore->semaphore, &ts); 
}

void ctsWaitPlatformSemaphores(
    uint32_t semaphoreCount,
    const CtsPlatformSemaphore* pSemaphores
) {
    int currentValue;
    for (uint32_t i = 0; i < semaphoreCount; ++i) {
        sem_getvalue(&pSemaphores[i].semaphore, &currentValue);

        if (currentValue < pSemaphores[i].maxValue) {
            sem_post(&pSemaphores[i].semaphore); 
        }
    }
}

#ifdef __cplusplus
}
#endif