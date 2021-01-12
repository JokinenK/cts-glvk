#include <cts/align.h>
#include <windows.h>
#include <string.h>
#include <cts/allocator.h>
#include <cts/thread.h>
#include <private/thread_private.h>

#ifdef __cplusplus
extern "C" {
#endif

static DWORD WINAPI threadEntry(LPVOID pArgs)
{
    CtsThreadCreateInfo createInfo;
    memcpy(&createInfo, pArgs, sizeof(CtsThreadCreateInfo));

    createInfo.entryPoint(createInfo.args);
    return 0;
}

CtsResult ctsCreateThread(
    const CtsThreadCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsThread* pThread
) {
    CtsThread thread = ctsAllocation(
        pAllocator,
        sizeof(struct CtsThread),
        alignof(struct CtsThread),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (!thread) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    thread->joined = false;
    thread->detached = false;
    thread->thread = CreateThread(
        NULL,                 /* default security attributes */
        0,                    /* default stack size */    
        threadEntry,          /* thread function */
        (LPVOID) pCreateInfo, /* parameter to thread function */
        0,                    /* default creation flags */ 
        &thread->threadId
    );

    *pThread = thread;
    return CTS_SUCCESS;
}

void ctsDestroyThread(
    CtsThread pThread,
    const CtsAllocationCallbacks* pAllocator
) {
    if (pThread) {
        ctsThreadJoin(pThread);
        ctsFree(pAllocator, pThread);
    }
}

bool ctsThreadJoinable(CtsThread pThread)
{
    return (!pThread->joined && !pThread->detached);
}

void ctsThreadJoin(
    CtsThread pThread
) {
    if (ctsThreadJoinable(pThread)) {
        pThread->joined = true;
        WaitForSingleObject(pThread->thread, INFINITE);
        CloseHandle(pThread->thread);
    }
}

void ctsThreadDetach(
    CtsThread pThread
) {
    if (ctsThreadJoinable(pThread)) {
        pThread->detached = true;
        CloseHandle(pThread->thread);
    }
}

#ifdef __cplusplus
}
#endif