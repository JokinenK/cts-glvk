#pragma once

#include <stdint.h>
#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDestroyPipeline {
    CtsCmdBase base;
    CtsDevice device;
    CtsPipeline pipeline;
    const CtsAllocationCallbacks* allocator;
} CtsDestroyPipeline;

#ifdef __cplusplus
}
#endif