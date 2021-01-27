#include <cts/platform_condition_variable.h>
#include <cts/platform_mutex.h>

#ifdef __cplusplus
extern "C" {
#endif

bool ctsInitPlatformConditionVariable(
    CtsPlatformConditionVariable* pConditionVariable
) {
    pthread_cond_init(pConditionVariable, NULL);
}

void ctsDestroyPlatformConditionVariable(
    CtsPlatformConditionVariable* pConditionVariable
) {
}

void ctsSleepPlatformConditionVariable(
    CtsPlatformConditionVariable* pConditionVariable,
    CtsPlatformMutex* pMutex
) {
    if (pConditionVariable != NULL && pMutex != NULL) {
        pthread_cond_wait(pConditionVariable, pMutex);
    }
}

void ctWakePlatformsConditionVariable(
    CtsPlatformConditionVariable* pConditionVariable
) {
    pthread_cond_signal(pConditionVariable);
}

void ctsWakeAllPlatformConditionVariable(
    CtsPlatformConditionVariable* pConditionVariable
) {
    pthread_cond_broadcast(pConditionVariable);
}

#ifdef __cplusplus
}
#endif