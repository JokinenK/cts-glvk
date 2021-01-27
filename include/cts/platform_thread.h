#pragma once

#include <stdbool.h>
#include <cts/platform/thread_type.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void(*CtsPlatformThreadEntryPoint)(void* pArgs);

void ctsInitPlatformThread(
    CtsPlatformThread* pThread,
    CtsPlatformThreadEntryPoint entryPoint,
    void* pArgs
);

void ctsDestroyPlatformThread(
    CtsPlatformThread* pThread
);

bool ctsJoinablePlatformThread(
    CtsPlatformThread* pThread
);

void ctsJoinPlatformThread(
    CtsPlatformThread* pThread
);

void ctsDetachPlatformThread(
    CtsPlatformThread* pThread
);

#ifdef __cplusplus
}
#endif