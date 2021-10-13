#pragma once

#include "vulkan/vulkan_core.h"
#include <private/physical_device_private.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsInstance {
    struct CtsPhysicalDevice physicalDevice;
};

#ifdef __cplusplus
}
#endif