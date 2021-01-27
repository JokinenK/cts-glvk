#pragma once

#include <stdbool.h>
#include <cts/platform_mutex.h>
#include <cts/platform/condition_variable_type.h>

#ifdef __cplusplus
extern "C" {
#endif

bool ctsInitPlatformConditionVariable(
    CtsPlatformConditionVariable* pConditionVariable
);

void ctsDestroyPlatformConditionVariable(
    CtsPlatformConditionVariable* pConditionVariable
);

void ctsSleepPlatformConditionVariable(
    CtsPlatformConditionVariable* pConditionVariable,
    CtsPlatformMutex* pMutex
);

void ctWakePlatformsConditionVariable(
    CtsPlatformConditionVariable* pConditionVariable
);

void ctsWakeAllPlatformConditionVariable(
    CtsPlatformConditionVariable* pConditionVariable
);

#ifdef __cplusplus
}
#endif