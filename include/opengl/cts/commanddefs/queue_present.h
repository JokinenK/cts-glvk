#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsQueuePresent {
    CtsCmdBase base;
    VkQueue queue;
    const VkPresentInfoKHR* pPresentInfo;
    VkResult* pResult;
} CtsQueuePresent;

#ifdef __cplusplus
}
#endif
