#include <stddef.h>
#include <string.h>
#include <cts/shader_module.h>
#include <private/shader_module_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateShaderModule(
    CtsDevice device,
    const CtsShaderModuleCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsShaderModule* pShaderModule
) {
    (void) device;

    CtsShaderModule shaderModule = ctsAllocation(
        pAllocator,
        sizeof(struct CtsShaderModuleImpl),
        alignof(struct CtsShaderModuleImpl),
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

    memcpy(shaderModule->code, pCreateInfo->pCode, pCreateInfo->codeSize);
    *pShaderModule = shaderModule;
    return CTS_SUCCESS;
}

void ctsDestroyShaderModule(
    CtsDevice device,
    CtsShaderModule shaderModule,
    const CtsAllocationCallbacks* pAllocator
) {
    (void) device;

    if (shaderModule != NULL) {
        ctsFree(pAllocator, shaderModule->code);
        ctsFree(pAllocator, shaderModule);
    }
}

#ifdef __cplusplus
}
#endif