#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <cts/allocator.h>
#include <cts/typedefs/device.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/semaphore.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateSemaphore(
    CtsDevice pDevice,
    const CtsSemaphoreCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsSemaphore* pSemaphore
);

void ctsDestroySemaphore(
    CtsDevice pDevice,
    CtsSemaphore pSemaphore,
    const CtsAllocationCallbacks* pAllocator
);

void ctsWaitSemaphores(
    uint32_t pSemaphoreCount,
    const CtsSemaphore* pSemaphores
);

void ctsSignalSemaphores(
    uint32_t pSemaphoreCount,
    const CtsSemaphore* pSemaphores
);

#ifdef __cplusplus
}
#endif