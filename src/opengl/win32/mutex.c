#include <cts/align.h> 
#include <windows.h> 
#include <cts/mutex.h>
#include <private/mutex_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateMutexes(
    const CtsMutexCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsMutex* pMutex
) {
    CtsMutex mutex = ctsAllocation(
        pAllocator,
        sizeof(struct CtsMutex),
        alignof(struct CtsMutex),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (mutex == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    InitializeCriticalSection(&mutex->criticalSection);
    *pMutex = mutex;

    return CTS_SUCCESS;
}

void ctsDestroyMutex(
    CtsMutex mutex,
    const CtsAllocationCallbacks* pAllocator
) {
    if (mutex != NULL) {
        DeleteCriticalSection(&mutex->criticalSection);
        ctsFree(pAllocator, mutex);
    }
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