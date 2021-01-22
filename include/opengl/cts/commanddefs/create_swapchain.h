#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCreateSwapchain {
    CtsCmdBase base;
    CtsDevice device;
    const CtsSwapchainCreateInfo* pCreateInfo;
    const CtsAllocationCallbacks* pAllocator;
    CtsSwapchain* pSwapchain;
    CtsResult* pResult;
} CtsCreateSwapchain;

#ifdef __cplusplus
}
#endif
