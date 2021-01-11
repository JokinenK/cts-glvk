#pragma once

#include <stdint.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateShaderModule(
    CtsDevice pDevice,
    const CtsShaderModuleCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsShaderModule* pShaderModule
);

void ctsDestroyShaderModule(
    CtsDevice pDevice,
    CtsShaderModule pShaderModule,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif