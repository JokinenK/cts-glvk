#include <cts/align.h> 
#include <windows.h> 
#include <cts/mutex.h>
#include <private/mutex_private.h>

#ifdef __cplusplus
extern "C" {
#endif

bool ctsCreateMutexes(
    const CtsMutexCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    uint32_t mutexCount,
    CtsMutex* pMutexes
) {
    for (uint32_t i = 0; i < mutexCount; ++i) {
        CtsMutex mutex = ctsAllocation(
            pAllocator,
            sizeof(struct CtsMutex),
            alignof(struct CtsMutex),
            CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
        );

        if (!mutex) {
            return false;
        }

        InitializeCriticalSection(&mutex->criticalSection);
        pMutexes[i] = mutex;
    }

    return true;
}

bool ctsDestroyMutex(
    CtsMutex mutex,
    const CtsAllocationCallbacks* pAllocator
) {
    if (mutex) {
        DeleteCriticalSection(&mutex->criticalSection);
        ctsFree(pAllocator, mutex);
        return true;
    }

    return false;
}

void ctsMutexLock(
    CtsMutex mutex
) {
    EnterCriticalSection(&mutex->criticalSection);
}

bool ctsMutexTryLock(
    CtsMutex mutex
) {
    return TryEnterCriticalSection(&mutex->criticalSection);
}

void ctsMutexUnlock(
    CtsMutex mutex
) {
    LeaveCriticalSection(&mutex->criticalSection);
}

#ifdef __cplusplus
}
#endif