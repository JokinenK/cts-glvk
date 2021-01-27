#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <cts/platform/semaphore_type.h>

#ifdef __cplusplus
extern "C" {
#endif

void ctsInitPlatformSemaphore(
    CtsPlatformSemaphore* pSemaphore,
    uint32_t initialValue,
    uint32_t maxValue
);

void ctsDestroyPlatformSemaphore(
    CtsPlatformSemaphore* pSemaphore
);

bool ctsWaitPlatformSemaphore(
    CtsPlatformSemaphore* pSemaphore,
    uint64_t timeout
);

void ctsWaitPlatformSemaphores(
    uint32_t semaphoreCount,
    const CtsPlatformSemaphore* pSemaphores
);

void ctsSignalPlatformSemaphores(
    uint32_t semaphoreCount,
    const CtsPlatformSemaphore* pSemaphores
);

#ifdef __cplusplus
}
#endif