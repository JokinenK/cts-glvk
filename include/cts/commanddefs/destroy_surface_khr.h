#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/cmd_base.h"
#include "cts/surface_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDestroySurfaceKHR {
    CtsCmdBase base;
    VkInstance instance;
    VkSurfaceKHR surface;
    const VkAllocationCallbacks* pAllocator;
} CtsDestroySurfaceKHR;

CTS_DEFINE_TRAMPOLINE_VOID(
    CtsDestroySurfaceKHR,
    ctsDestroySurfaceKHRImpl,
    instance,
    surface,
    pAllocator
);

#ifdef __cplusplus
}
#endif
