#pragma once

#include "vulkan/vulkan_core.h"
#include "private/physical_device_private.h"
#include "private/object_base.h"

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