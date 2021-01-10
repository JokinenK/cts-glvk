#include <cts/align.h>
#include <pthread.h>
#include <string.h>
#include <cts/allocator.h>
#include <cts/thread.h>
#include <private/thread_private.h>

#ifdef __cplusplus
extern "C" {
#endif

static void* threadEntry(void* pArgs)
{
    struct CtsThreadCreateInfo createInfo;
    memcpy(&createInfo, pArgs, sizeof(CtsThreadCreateInfo));

    createInfo.entryPoint(createInfo.args);
    return NULL;
}

bool ctsCreateThreads(
    const CtsThreadCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    uint32_t pThreadCount,
    CtsThread* pThreads
) {
    for (uint32_t i = 0; i < pThreadCount; ++i) {
        CtsThread thread = ctsAllocation(
            pAllocator,
            sizeof(struct CtsThread),
            alignof(struct CtsThread),
            CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
        );

        if (!thread) {
            return false;
        }

        thread->joined = false;
        thread->detached = false;
        pthread_create(&thread->thread, NULL, threadEntry, pCreateInfo); 
        
        pThreads[i] = thread;
        
    }

    return true;
}

bool ctsDestroyThread(
    CtsThread pThread,
    const CtsAllocationCallbacks* pAllocator
) {
    if (pThread) {
        ctsThreadJoin(pThread);
        ctsFree(pAllocator, pThread);
        return true; 
    }

    return false;
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
        pthread_join(pThread->thread, NULL);
    }
}

void ctsThreadDetach(
    CtsThread pThread
) {
    if (ctsThreadJoinable(pThread)) {
        pThread->detached = true;
        pthread_detach(pThread->thread);
    }
}

#ifdef __cplusplus
}
#endif