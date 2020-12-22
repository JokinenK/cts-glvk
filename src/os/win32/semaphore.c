#include <windows.h>
#include <climits>
#include <cts/align.h>
#include <cts/allocator.h>
#include <cts/os/semaphore.h>
#include <private/semaphore_private.h>

#ifdef __cplusplus
extern "C" {
#endif

bool ctsCreateSemaphores(
    const CtsSemaphoreCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    uint32_t pSemaphoreCount,
    CtsSemaphore* pSemaphores
) {
    for (uint32_t i = 0; i < pSemaphoreCount; ++i) {
        CtsSemaphore semaphore = ctsAllocation(
            pAllocator,
            sizeof(struct CtsSemaphore),
            alignof(struct CtsSemaphore),
            CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
        );

        if (!semaphore) {
            return false;
        }
        semaphore->semaphore = CreateSemaphore( 
            NULL,                            // default security attributes
            (LONG)pCreateInfo->initialValue, // initial count
            LONG_MAX,                        // maximum count
            NULL                             // unnamed semaphore
        );

        pSemaphores[i] = semaphore;
    }

    return true;
}

bool ctsDestroySemaphore(
    CtsSemaphore pSemaphore,
    const CtsAllocationCallbacks* pAllocator
) {
    if (pSemaphore) {
        CloseHandle(pSemaphore->semaphore); 
        ctsFree(pAllocator, pSemaphore);
        return true;
    }
    
    return false;
}

void ctsSemaphoreWait(
    CtsSemaphore pSemaphore
) {
    WaitForSingleObject(pSemaphore->semaphore, INFINITE);
}

bool ctsSemaphoreTryWait(
    CtsSemaphore pSemaphore
) {
    return (WaitForSingleObject(&pSemaphore->semaphore, 0) == WAIT_OBJECT_0);
}

void ctsSemaphorePost(
    CtsSemaphore pSemaphore
) {
    ReleaseSemaphore(pSemaphore->semaphore, 1, NULL);
}

#ifdef __cplusplus
}
#endif