#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/fence_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsGetFenceStatus {
    CtsCmdBase base;
    VkDevice device;
    VkFence fence;
    VkResult* pResult;
} CtsGetFenceStatus;

CTS_DEFINE_COMMAND_TRAMPOLINE(
    CtsGetFenceStatus,
    ctsGetFenceStatusImpl,
    device,
    fence
);

CTS_DEFINE_COMMAND_METADATA(CtsGetFenceStatus);

#ifdef __cplusplus
}
#endif