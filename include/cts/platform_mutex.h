#pragma once

#include <stdbool.h>
#include <cts/platform/mutex_type.h>

#ifdef __cplusplus
extern "C" {
#endif

void ctsInitPlatformMutex(CtsPlatformMutex* pMutex);
void ctsDestroyPlatformMutex(CtsPlatformMutex* pMutex);

void ctsLockPlatformMutex(CtsPlatformMutex* pMutex);
bool ctsTryLockPlatformMutex(CtsPlatformMutex* pMutex);
void ctsUnlockPlatformMutex(CtsPlatformMutex* pMutex);

#ifdef __cplusplus
}
#endif