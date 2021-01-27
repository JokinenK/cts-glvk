#pragma once

#include <stdint.h>
#include <cts/allocator.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateInstance(
    const CtsInstanceCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsInstance* pInstance
);

CtsResult ctsDestroyInstance(
    CtsInstance instance,
    const CtsAllocationCallbacks* pAllocator
);

CtsResult ctsEnumeratePhysicalDevices(
    CtsInstance instance,
    uint32_t* pPhysicalDeviceCount,
    CtsPhysicalDevice* pPhysicalDevices
);

#ifdef __cplusplus
}
#endif