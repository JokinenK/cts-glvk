#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/sampler_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCreateSampler {
    CtsCmdBase base;
    VkDevice device;
    const VkSamplerCreateInfo* pCreateInfo;
    const VkAllocationCallbacks* pAllocator;
    VkSampler* pSampler;
    VkResult* pResult;
} CtsCreateSampler;

CTS_DEFINE_COMMAND_TRAMPOLINE(
    CtsCreateSampler,
    ctsCreateSamplerImpl,
    device,
    pCreateInfo,
    pAllocator,
    pSampler
);

CTS_DEFINE_COMMAND_METADATA(CtsCreateSampler);

#ifdef __cplusplus
}
#endif