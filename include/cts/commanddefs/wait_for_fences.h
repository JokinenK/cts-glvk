#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/fence_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsWaitForFences {
    CtsCmdBase base;
    VkDevice device;
    uint32_t fenceCount;
    const VkFence* pFences;
    VkBool32 waitAll;
    uint64_t timeout;
    VkResult* pResult;
} CtsWaitForFences;

CTS_DEFINE_TRAMPOLINE(
    CtsWaitForFences,
    ctsWaitForFencesImpl,
    device,
    fenceCount,
    pFences,
    waitAll,
    timeout
);

#ifdef __cplusplus
}
#endif