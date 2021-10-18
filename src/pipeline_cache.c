#include <stddef.h>
#include "cts/pipeline_cache.h"
#include "cts/private.h"

#ifdef __cplusplus
extern "C" {
#endif

VkResult VKAPI_CALL ctsCreatePipelineCache(
    VkDevice device,
    const VkPipelineCacheCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkPipelineCache* pPipelineCache
) {
    *pPipelineCache = CtsPipelineCacheToHandle(NULL);
    return VK_SUCCESS;
}

void VKAPI_CALL ctsDestroyPipelineCache(
    VkDevice device,
    VkPipelineCache pipelineCache,
    const VkAllocationCallbacks* pAllocator
) {

}

#ifdef __cplusplus
}
#endif