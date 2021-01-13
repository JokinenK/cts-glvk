#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCreateSampler {
    CtsCmdBase base;
    CtsDevice device;
    const CtsSamplerCreateInfo* pCreateInfo;
    const CtsAllocationCallbacks* pAllocator;
    CtsSampler* pSampler;
    CtsResult* pResult;
} CtsCreateSampler;

#ifdef __cplusplus
}
#endif