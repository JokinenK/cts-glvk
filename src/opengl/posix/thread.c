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

    createInfo.pfEntryPoint(createInfo.pArgs);
    return NULL;
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
    pthread_create(&thread->thread, NULL, threadEntry, pCreateInfo); 
    
    *pThread = thread;
    return CTS_SUCCESS;
}

void ctsDestroyThread(
    CtsThread thread,
    const CtsAllocationCallbacks* pAllocator
) {
    if (thread != NULL) {
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
        pthread_join(thread->thread, NULL);
    }
}

void ctsThreadDetach(
    CtsThread thread
) {
    if (ctsThreadJoinable(thread)) {
        thread->detached = true;
        pthread_detach(thread->thread);
    }
}

#ifdef __cplusplus
}
#endif