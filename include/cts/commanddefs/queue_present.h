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

CTS_DEFINE_COMMAND_TRAMPOLINE(
    CtsQueuePresentKHR,
    ctsQueuePresentKHRImpl,
    queue,
    pPresentInfo
);

CTS_DEFINE_COMMAND_METADATA(CtsQueuePresentKHR);

#ifdef __cplusplus
}
#endif
