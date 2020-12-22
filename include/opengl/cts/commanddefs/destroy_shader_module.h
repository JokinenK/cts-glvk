#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDestroyShaderModule {
    CtsCmdBase base;
    CtsDevice device;
    CtsShaderModule shaderModule;
    const CtsAllocationCallbacks* allocator;
} CtsDestroyShaderModule;

#ifdef __cplusplus
}
#endif