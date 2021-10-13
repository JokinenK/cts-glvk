#pragma once

#include "cts/platform/platform_semaphore.h"
#include "private/object_base.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsSemaphore {
    struct CtsObjectBase base;
    CtsPlatformSemaphore semaphore;
};

VkResult ctsWaitSemaphore(
    VkSemaphore semaphore,
    uint64_t timeout
);

void ctsWaitSemaphores(
    uint32_t semaphoreCount,
    const VkSemaphore* pSemaphores
);

void ctsSignalSemaphores(
    uint32_t semaphoreCount,
    const VkSemaphore* pSemaphores
);

#ifdef __cplusplus
}
#endif