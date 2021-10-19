#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/swapchain_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsQueuePresentKHR {
    CtsCmdBase base;
    VkQueue queue;
    const VkPresentInfoKHR* pPresentInfo;
    VkResult* pResult;
} CtsQueuePresentKHR;

CTS_DEFINE_TRAMPOLINE(
    CtsQueuePresentKHR,
    ctsQueuePresentKHRImpl,
    queue,
    pPresentInfo
);

#ifdef __cplusplus
}
#endif
