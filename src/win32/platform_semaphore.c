#include <cts/platform_semaphore.h>

#ifdef __cplusplus
extern "C" {
#endif

void ctsInitPlatformSemaphore(
    CtsPlatformSemaphore* pSemaphore,
    uint32_t initialValue,
    uint32_t maxValue
) {
    (*pSemaphore) = CreateSemaphoreA( 
        NULL,               // default security attributes
        (LONG)initialValue, // initial count
        (LONG)maxValue,     // maximum count
        NULL                // unnamed semaphore
    );
}

void ctsDestroyPlatformSemaphore(
    CtsPlatformSemaphore* pSemaphore
) {
    if (pSemaphore != NULL) {
        CloseHandle(*pSemaphore); 
    }
}

bool ctsWaitPlatformSemaphore(
    CtsPlatformSemaphore* pSemaphore,
    uint64_t timeout
) {
    DWORD timeoutMillis = (timeout < UINT64_MAX)
        ? (DWORD)(timeout / 1000000ULL)
        : INFINITE;

    if (WaitForSingleObject(*pSemaphore, timeoutMillis) == WAIT_OBJECT_0) {
        return true;
    }

    return false;
}

void ctsWaitPlatformSemaphores(
    uint32_t semaphoreCount,
    const CtsPlatformSemaphore* pSemaphores
) {
    WaitForMultipleObjects(
        (DWORD)semaphoreCount,
        pSemaphores,
        true,
        INFINITE
    );
}

void ctsSignalPlatformSemaphores(
    uint32_t semaphoreCount,
    const CtsPlatformSemaphore* pSemaphores
) {
    for (uint32_t i = 0; i < semaphoreCount; ++i) {
        ReleaseSemaphore(pSemaphores[i], 1, NULL);
    }
}

#ifdef __cplusplus
}
#endif