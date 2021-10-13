#pragma once

#include "vulkan/vk_icd.h"
#include "cts/macros.h"

#ifdef __cplusplus
extern "C" {
#endif

LIBRARY_API PFN_vkVoidFunction VKAPI_CALL vk_icdGetInstanceProcAddr(VkInstance instance, const char* pName);
LIBRARY_API VkResult VKAPI_CALL vk_icdNegotiateLoaderICDInterfaceVersion(uint32_t* pSupportedVersion);

#ifdef __cplusplus
}
#endif
