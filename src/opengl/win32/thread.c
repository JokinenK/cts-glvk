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
    CtsThreadCreateInfo* pCreateInfo = pArgs;
    pCreateInfo->pfEntryPoint(pCreateInfo->pArgs);
    return 0;
}

CtsResult ctsCreateThread(
    const CtsThreadCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsThread* pThread
) {
    CtsThread thread = ctsAllocation(
        pAllocator,
        sizeof(struct CtsThreadImpl),
        alignof(struct CtsThreadImpl),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (!thread) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    thread->joined = false;
    thread->detached = false;
    memcpy(&thread->createInfo, pCreateInfo, sizeof(CtsThreadCreateInfo));
    thread->thread = CreateThread(
        NULL,                 /* default security attributes */
        0,                    /* default stack size */    
        threadEntry,          /* thread function */
        (LPVOID) &thread->createInfo, /* parameter to thread function */
        0,                    /* default creation flags */ 
        &thread->threadId
    );

    *pThread = thread;
    return CTS_SUCCESS;
}

void ctsDestroyThread(
    CtsThread thread,
    const CtsAllocationCallbacks* pAllocator
) {
    if (thread) {
        ctsThreadJoin(thread);
        ctsFree(pAllocator, thread);
    }
}

bool ctsThreadJoinable(CtsThread thread)
{
    return (!thread->joined && !thread->detached);
}

void ctsThreadJoin(
    CtsThread thread
) {
    if (ctsThreadJoinable(thread)) {
        thread->joined = true;
        WaitForSingleObject(thread->thread, INFINITE);
        CloseHandle(thread->thread);
    }
}

void ctsThreadDetach(
    CtsThread thread
) {
    if (ctsThreadJoinable(thread)) {
        thread->detached = true;
        CloseHandle(thread->thread);
    }
}

#ifdef __cplusplus
}
#endif