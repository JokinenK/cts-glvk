#include <stddef.h>
#include <string.h>
#include <cts/shader_module.h>
#include <private/shader_module_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateShaderModule(
    CtsDevice pDevice,
    const CtsShaderModuleCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsShaderModule* pShaderModule
) {
    (void) pDevice;

    CtsShaderModule shaderModule = ctsAllocation(
        pAllocator,
        sizeof(struct CtsShaderModule),
        alignof(struct CtsShaderModule),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (shaderModule == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    shaderModule->codeSize = pCreateInfo->codeSize;
    shaderModule->code = ctsAllocation(
        pAllocator,
        sizeof(char) * pCreateInfo->codeSize,
        alignof(char),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    memcpy(shaderModule->code, pCreateInfo->code, pCreateInfo->codeSize);
    *pShaderModule = shaderModule;
    return CTS_SUCCESS;
}

void ctsDestroyShaderModule(
    CtsDevice pDevice,
    CtsShaderModule pShaderModule,
    const CtsAllocationCallbacks* pAllocator
) {
    (void) pDevice;

    if (pShaderModule != NULL) {
        ctsFree(pAllocator, pShaderModule->code);
        ctsFree(pAllocator, pShaderModule);
    }
}

#ifdef __cplusplus
}
#endif