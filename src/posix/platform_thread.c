#include <cts/platform_thread.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ThreadArgs {
    CtsPlatformThreadEntryPoint entryPoint;
    void* pArgs;
} ThreadArgs;

static void* threadEntry(void* pArgs)
{
    ThreadArgs* pThreadArgs = pArgs;
    pThreadArgs->entryPoint(pThreadArgs->pArgs);
    return NULL;
}

void ctsInitPlatformThread(
    CtsPlatformThread* pThread,
    CtsPlatformThreadEntryPoint entryPoint,
    void* pArgs
) {
    ThreadArgs threadArgs = {
        .entryPoint = entryPoint,
        .pArgs = pArgs
    };

    pthread_create(pThread, NULL, threadEntry, &threadArgs); 
}

void ctsDestroyPlatformThread(
    CtsPlatformThread* pThread
) {
    ctsThreadJoin(pThread);
}

bool ctsJoinablePlatformThread(
    CtsPlatformThread* pThread
) {
    return (pthread_equal(pthread_self(), *pThread) == 0);
}

void ctsJoinPlatformThread(
    CtsPlatformThread* pThread
) {
    if (ctsJoinablePlatformThread(pThread)) {
        pthread_join(pThread, NULL);
    }
}

void ctsDetachPlatformThread(
    CtsPlatformThread* pThread
) {
    if (ctsJoinablePlatformThread(pThread)) {
        pthread_detach(pThread);
    }
}

#ifdef __cplusplus
}
#endif