#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <cts/allocator.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsSemaphore* CtsSemaphore;
typedef struct CtsSemaphoreCreateInfo {
    size_t initialValue;
} CtsSemaphoreCreateInfo;

bool ctsCreateSemaphores(
    const CtsSemaphoreCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    uint32_t pSemaphoreCount,
    CtsSemaphore* pSemaphores
);

bool ctsDestroySemaphore(
    CtsSemaphore pSemaphore,
    const CtsAllocationCallbacks* pAllocator
);

void ctsSemaphoreWait(
    CtsSemaphore pSemaphore
);

bool ctsSemaphoreTryWait(
    CtsSemaphore pSemaphore
);

void ctsSemaphorePost(
    CtsSemaphore pSemaphore
);

#ifdef __cplusplus
}
#endif