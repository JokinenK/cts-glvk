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

CTS_DEFINE_COMMAND_TRAMPOLINE_VOID(
    CtsDestroySampler,
    ctsDestroySamplerImpl,
    device,
    sampler,
    pAllocator
);

CTS_DEFINE_COMMAND_METADATA(CtsDestroySampler);

#ifdef __cplusplus
}
#endif