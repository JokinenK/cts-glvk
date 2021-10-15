#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/physical_device_private.h"
#include "cts/object_base.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsInstance {
    struct CtsObjectBase base;
    struct CtsPhysicalDevice physicalDevice;
};

#ifdef __cplusplus
}
#endif