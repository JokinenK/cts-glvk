#pragma once

#include <Windows.h>
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_win32.h"
#include "cts/commanddefs/cmd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCreateWin32SurfaceKHR {
    CtsCmdBase base;
    VkInstance instance;
    const VkWin32SurfaceCreateInfoKHR* pCreateInfo;
    const VkAllocationCallbacks* pAllocator;
    VkSurfaceKHR* pSurface;
    VkResult* pResult;
} CtsCreateWin32SurfaceKHR;

#ifdef __cplusplus
}
#endif