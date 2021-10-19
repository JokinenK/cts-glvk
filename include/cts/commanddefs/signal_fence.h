#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/fence_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsSignalFence {
    CtsCmdBase base;
    VkDevice device;
    VkFence fence;
} CtsSignalFence;

CTS_DEFINE_COMMAND_TRAMPOLINE_VOID(
    CtsSignalFence,
    ctsSignalFenceImpl,
    device,
    fence
);

CTS_DEFINE_COMMAND_METADATA(CtsSignalFence);

#ifdef __cplusplus
}
#endif