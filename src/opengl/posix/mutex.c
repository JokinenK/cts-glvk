#include <pthread.h> 
#include <cts/align.h> 
#include <cts/mutex.h>
#include <private/mutex_private.h>

#ifdef __cplusplus
extern "C" {
#endif

bool ctsCreateMutexes(
    const CtsMutexCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    uint32_t pMutexCount,
    CtsMutex* pMutexes
) {
    for (uint32_t i = 0; i < pMutexCount; ++i) {
        CtsMutex mutex = ctsAllocation(
            pAllocator,
            sizeof(struct CtsMutex),
            alignof(struct CtsMutex),
            CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
        );

        if (!mutex) {
            return false;
        }

        pthread_mutex_init(&mutex->mutex, NULL);
        pMutexes[i] = mutex;
    }

    return true;
}

bool ctsDestroyMutex(
    CtsMutex pMutex,
    const CtsAllocationCallbacks* pAllocator
) {
    if (pMutex) {
        pthread_mutex_destroy(&pMutex->mutex);
        ctsFree(pAllocator, pMutex);
        return true;
    }

    return false;
}

void ctsMutexLock(
    CtsMutex pMutex
) {
    pthread_mutex_lock(&pMutex->mutex);
}

bool ctsMutexTryLock(
    CtsMutex pMutex
) {
    return (pthread_mutex_trylock(&pMutex->mutex) == 0);
}

void ctsMutexUnlock(
    CtsMutex pMutex
) {
    pthread_mutex_unlock(&pMutex->mutex);
}

#ifdef __cplusplus
}
#endif