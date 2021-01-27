#include <cts/platform_mutex.h>

#ifdef __cplusplus
extern "C" {
#endif

void ctsInitPlatformMutex(CtsPlatformMutex* pMutex) {
    InitializeCriticalSection(pMutex);
}

void ctsDestroyPlatformMutex(CtsPlatformMutex* pMutex) {
    DeleteCriticalSection(pMutex);
}

void ctsLockPlatformMutex(CtsPlatformMutex* pMutex) {
    EnterCriticalSection(pMutex);
}

bool ctsTryLockPlatformMutex(CtsPlatformMutex* pMutex) {
    return TryEnterCriticalSection(pMutex);
}

void ctsUnlockPlatformMutex(CtsPlatformMutex* pMutex) {
    LeaveCriticalSection(pMutex);
}

#ifdef __cplusplus
}
#endif