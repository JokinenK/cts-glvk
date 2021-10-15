#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/fence_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsResetFences {
    CtsCmdBase base;
    VkDevice device;
    uint32_t fenceCount;
    const VkFence* pFences;
    VkResult* pResult;
} CtsResetFences;

CTS_DEFINE_TRAMPOLINE(
    ResetFences,
    device,
    fenceCount,
    pFences
);

#ifdef __cplusplus
}
#endif