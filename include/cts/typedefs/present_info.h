#pragma once

#include <cts/typedefs/semaphore.h>
#include <cts/typedefs/swapchain.h>
#include <cts/typedefs/enums.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsPresentInfo {
    CtsStructureType sType;
    const void* next;
    uint32_t waitSemaphoreCount;
    const CtsSemaphore* waitSemaphores;
    uint32_t swapchainCount;
    const CtsSwapchain* swapchains;
    const uint32_t* imageIndices;
    CtsResult* results;
} CtsPresentInfo;

#ifdef __cplusplus
}
#endif
