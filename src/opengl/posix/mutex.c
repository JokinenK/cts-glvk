#include <pthread.h> 
#include <cts/align.h> 
#include <cts/mutex.h>
#include <private/mutex_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateMutex(
    const CtsMutexCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsMutex* pMutex
) {
    CtsMutex mutex = ctsAllocation(
        pAllocator,
        sizeof(struct CtsMutexImpl),
        alignof(struct CtsMutexImpl),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (mutex == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    pthread_mutex_init(&mutex->mutex, NULL);
    *pMutex = mutex;

    return CTS_SUCCESS;
}

void ctsDestroyMutex(
    CtsMutex mutex,
    const CtsAllocationCallbacks* pAllocator
) {
    if (mutex != NULL) {
        pthread_mutex_destroy(&mutex->mutex);
        ctsFree(pAllocator, pMutex);
    }
}

void ctsMutexLock(
    CtsMutex mutex
) {
    pthread_mutex_lock(&mutex->mutex);
}

bool ctsMutexTryLock(
    CtsMutex mutex
) {
    return (pthread_mutex_trylock(&mutex->mutex) == 0);
}

void ctsMutexUnlock(
    CtsMutex mutex
) {
    pthread_mutex_unlock(&mutex->mutex);
}

#ifdef __cplusplus
}
#endif