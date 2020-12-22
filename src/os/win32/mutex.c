#include <cts/align.h> 
#include <windows.h> 
#include <cts/os/mutex.h>
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

        InitializeCriticalSection(&mutex->criticalSection);
        pMutexes[i] = mutex;
    }

    return true;
}

bool ctsDestroyMutex(
    CtsMutex pMutex,
    const CtsAllocationCallbacks* pAllocator
) {
    if (pMutex) {
        DeleteCriticalSection(&pMutex->criticalSection);
        ctsFree(pAllocator, pMutex);
        return true;
    }

    return false;
}

void ctsMutexLock(
    CtsMutex pMutex
) {
    EnterCriticalSection(&pMutex->criticalSection);
}

bool ctsMutexTryLock(
    CtsMutex pMutex
) {
    return TryEnterCriticalSection(&pMutex->criticalSection);
}

void ctsMutexUnlock(
    CtsMutex pMutex
) {
    LeaveCriticalSection(&pMutex->criticalSection);
}

#ifdef __cplusplus
}
#endif