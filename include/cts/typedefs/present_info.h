#pragma once

#include <cts/typedefs/semaphore.h>
#include <cts/typedefs/swapchain.h>
#include <cts/typedefs/enums.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsPresentInfo {
    CtsStructureType sType;
    const void* pNext;
    uint32_t waitSemaphoreCount;
    const CtsSemaphore* pWaitSemaphores;
    uint32_t swapchainCount;
    const CtsSwapchain* pSwapchains;
    const uint32_t* pImageIndices;
    CtsResult* pResults;
} CtsPresentInfo;

#ifdef __cplusplus
}
#endif
