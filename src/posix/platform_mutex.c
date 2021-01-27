#include <cts/platform_mutex.h>

#ifdef __cplusplus
extern "C" {
#endif

void ctsInitPlatformMutex(CtsPlatformMutex* pMutex) {
    pthread_mutex_init(pMutex, NULL);
}

void ctsDestroyPlatformMutex(CtsPlatformMutex* pMutex) {
    pthread_mutex_destroy(pMutex);
}

void ctsLockPlatformMutex(CtsPlatformMutex* pMutex) {
    pthread_mutex_lock(pMutex);
}

bool ctsTryLockPlatformMutex(CtsPlatformMutex* pMutex) {
    return (pthread_mutex_trylock(pMutex) == 0);
}

void ctsUnlockPlatformMutex(CtsPlatformMutex* pMutex) {
    pthread_mutex_unlock(pMutex);
}

#ifdef __cplusplus
}
#endif