#include "cts/platform/platform_condition_variable.h"
#include "cts/platform/platform_mutex.h"

#ifdef __cplusplus
extern "C" {
#endif


bool ctsInitPlatformConditionVariable(
    CtsPlatformConditionVariable* pConditionVariable
) {
    InitializeConditionVariable(pConditionVariable);
    return true;
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
        SleepConditionVariableCS(
            pConditionVariable,
            pMutex,
            INFINITE
        );
    }
}

void ctWakePlatformsConditionVariable(
    CtsPlatformConditionVariable* pConditionVariable
) {
    WakeConditionVariable(pConditionVariable);
}

void ctsWakeAllPlatformConditionVariable(
    CtsPlatformConditionVariable* pConditionVariable
) {
    WakeAllConditionVariable(pConditionVariable);
}

#ifdef __cplusplus
}
#endif