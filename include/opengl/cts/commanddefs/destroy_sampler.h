#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDestroySampler {
    CtsCmdBase base;
    CtsDevice device;
    CtsSampler sampler;
    const CtsAllocationCallbacks* allocator;
} CtsDestroySampler;

#ifdef __cplusplus
}
#endif