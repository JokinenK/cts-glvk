#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCreateShaderModule {
    CtsCmdBase base;
    CtsDevice device;
    const CtsShaderModuleCreateInfo* createInfo;
    const CtsAllocationCallbacks* allocator;
    CtsShaderModule* shaderModule;
    CtsResult* result;
} CtsCreateShaderModule;

#ifdef __cplusplus
}
#endif