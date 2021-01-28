#include <cts/platform_thread.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ThreadArgs {
    CtsPlatformThreadEntryPoint entryPoint;
    void* pArgs;
} ThreadArgs;

static DWORD WINAPI threadEntry(LPVOID pArgs)
{
    ThreadArgs* pThreadArgs = pArgs;
    pThreadArgs->entryPoint(pThreadArgs->pArgs);
    free(pThreadArgs);
    return 0;
}

void ctsInitPlatformThread(
    CtsPlatformThread* pThread,
    CtsPlatformThreadEntryPoint entryPoint,
    void* pArgs
) {
    ThreadArgs* pThreadArgs = malloc(sizeof(ThreadArgs));
    (*pThreadArgs) = (ThreadArgs) {
        .entryPoint = entryPoint,
        .pArgs = pArgs
    };

    DWORD threadId;
    (*pThread) = CreateThread(
        NULL,                 /* default security attributes */
        0,                    /* default stack size */    
        threadEntry,          /* thread function */
        (LPVOID) pThreadArgs, /* parameter to thread function */
        0,                    /* default creation flags */ 
        &threadId
    );
}

void ctsDestroyPlatformThread(
    CtsPlatformThread* pThread
) {
    ctsJoinPlatformThread(pThread);
}

bool ctsJoinablePlatformThread(
    CtsPlatformThread* pThread
) {
    return (*pThread != INVALID_HANDLE_VALUE);
}

void ctsJoinPlatformThread(
    CtsPlatformThread* pThread
) {
    if (ctsJoinablePlatformThread(pThread)) {
        WaitForSingleObject(*pThread, INFINITE);
        CloseHandle(*pThread);
        *pThread = INVALID_HANDLE_VALUE;
    }
}

void ctsDetachPlatformThread(
    CtsPlatformThread* pThread
) {
    if (ctsJoinablePlatformThread(pThread)) {
        CloseHandle(*pThread);
    }
}

#ifdef __cplusplus
}
#endif