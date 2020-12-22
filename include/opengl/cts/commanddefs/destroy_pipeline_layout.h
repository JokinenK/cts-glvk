#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDestroyPipelineLayout {
    CtsCmdBase base;
    CtsDevice device;
    CtsPipelineLayout pipelineLayout;
    const CtsAllocationCallbacks* allocator;
} CtsDestroyPipelineLayout;

#ifdef __cplusplus
}
#endif