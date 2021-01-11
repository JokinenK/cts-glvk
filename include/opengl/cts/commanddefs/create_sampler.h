#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCreateSampler {
    CtsCmdBase base;
    CtsDevice device;
    const CtsSamplerCreateInfo* createInfo;
    const CtsAllocationCallbacks* allocator;
    CtsSampler* sampler;
    CtsResult* result;
} CtsCreateSampler;

#ifdef __cplusplus
}
#endif