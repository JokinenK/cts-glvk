#pragma once

#include <stdint.h>
#include <cts/allocator.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateShaderModule(
    CtsDevice device,
    const CtsShaderModuleCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsShaderModule* pShaderModule
);

void ctsDestroyShaderModule(
    CtsDevice device,
    CtsShaderModule shaderModule,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif