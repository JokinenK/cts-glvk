#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/sampler_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDestroySampler {
    CtsCmdBase base;
    VkDevice device;
    VkSampler sampler;
    const VkAllocationCallbacks* pAllocator;
} CtsDestroySampler;

CTS_DEFINE_TRAMPOLINE_VOID(
    DestroySampler,
    device,
    sampler,
    pAllocator
);

#ifdef __cplusplus
}
#endif