#include <stddef.h>
#include <string.h>
#include <cts/shader_module.h>
#include <cts/allocator.h>
#include <cts/util/align.h>
#include <private/private.h>
#include <private/shader_module_private.h>

#ifdef __cplusplus
extern "C" {
#endif

VkResult VKAPI_CALL ctsCreateShaderModule(
    VkDevice deviceHandle,
    const VkShaderModuleCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkShaderModule* pShaderModule
) {
    (void) deviceHandle;

    struct CtsShaderModule* shaderModule = ctsAllocation(
        pAllocator,
        sizeof(struct CtsShaderModule),
        alignof(struct CtsShaderModule),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (shaderModule == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    set_loader_magic_value(shaderModule);

    shaderModule->codeSize = pCreateInfo->codeSize;
    shaderModule->code = ctsAllocation(
        pAllocator,
        sizeof(char) * pCreateInfo->codeSize,
        alignof(char),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    memcpy(shaderModule->code, pCreateInfo->pCode, pCreateInfo->codeSize);
    *pShaderModule = CtsShaderModuleToHandle(shaderModule);
    return VK_SUCCESS;
}

void VKAPI_CALL ctsDestroyShaderModule(
    VkDevice deviceHandle,
    VkShaderModule shaderModuleHandle,
    const VkAllocationCallbacks* pAllocator
) {
    (void) deviceHandle;

    struct CtsShaderModule* shaderModule = CtsShaderModuleFromHandle(shaderModuleHandle);

    if (shaderModule != NULL) {
        ctsFree(pAllocator, shaderModule->code);
        ctsFree(pAllocator, shaderModule);
    }
}

#ifdef __cplusplus
}
#endif